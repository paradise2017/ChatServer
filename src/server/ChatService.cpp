#include "ChatService.hpp"
#include "Public.hpp"
#include <muduo/base/Logging.h>
#include <vector>
#include <map>
using namespace std;
using namespace muduo;
ChatService *ChatService::GetInstance()
{
    static ChatService server;
    return &server;
}

ChatService::ChatService()
{
    // 注册消息以及对应的回调函数
    // 解耦的关键
    msg_handler_map_.insert({EnMsgType::LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    msg_handler_map_.insert({EnMsgType::REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    msg_handler_map_.insert({EnMsgType::ONE_CHAT_MSG, std::bind(&ChatService::Onchat, this, _1, _2, _3)});
    msg_handler_map_.insert({EnMsgType::ADD_FRIEND_MSG, std::bind(&ChatService::AddFriend, this, _1, _2, _3)});
    msg_handler_map_.insert({EnMsgType::LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});
    // 群组业务管理相关事件处理回调注册
    msg_handler_map_.insert({CREATE_GROUP_MSG, std::bind(&ChatService::CreateGroup, this, _1, _2, _3)});
    msg_handler_map_.insert({ADD_GROUP_MSG, std::bind(&ChatService::AddGroup, this, _1, _2, _3)});
    msg_handler_map_.insert({GROUP_CHAT_MSG, std::bind(&ChatService::GroupChat, this, _1, _2, _3)});

        // 连接redis服务器
    if (redis_.connect())
    {
        // 设置上报消息的回调  //redis 调用该类方法
        redis_.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}
MsgHandler ChatService::GetHandler(int msg_id)
{
    auto it = msg_handler_map_.find(msg_id);
    if (it == msg_handler_map_.end())
    {
        // 返回一个默认的处理器
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
        {
            LOG_ERROR << "msgid:" << msg_id << "can not find handler!"; // 记录错误日志，msgid没有对应的事件处理
        };
    }
    else
    {
        return msg_handler_map_[msg_id]; 
    }
}

// 由redis 调用
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
   //需要再判断，用户可能下线
    lock_guard<mutex> lock(conn_mutex_);
        auto it = user_conn_map_.find(userid);
        if (it != user_conn_map_.end()) //找到用户的connect
        {
            it->second->send(msg);
            return;
        }
 
        // 存储该用户的离线消息 
        offline_message_model_.insert(userid, msg);
}

void ChatService::reset()
{
    // 把online状态的用户，设置成offline
    user_model_.ResetState();
}

// 处理登录，  ORM 业务层操作的都是对象，DAO数据层
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = user_model_.query(id);

    if (user.GetId() != -1 && user.GetPassWord() == pwd)
    {
        if (user.GetState() == "online")
        {
            json response; // 登陆失败
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;                                        
            response["errmsg"] = "this account is using, input another!"; // 如果为-1表示注册失败
            conn->send(response.dump());                                  // 返回客户端
        }
        else
        {
            json response; // 登陆成功 ，更新用户状态信息
            {
                lock_guard<mutex> lock(conn_mutex_);
                // 数据库的并发由mysql保证
                user_conn_map_.insert({id, conn}); // 登录成功，记录用户连接信息  //并发登录
            }

            // id用户登录成功后，向redis订阅channel(id)     //其他服务器可能会发送消息，一个客户端一个id
            redis_.subscribe(id); 

            user.SetState("online");
            user_model_.UpdateState(user);
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0; // 如果为-1表示注册失败
            response["id"] = user.GetId();
            response["name"] = user.GetName();
            // 查询该用户是否有离线消息，
            vector<string> vec = offline_message_model_.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec;
                offline_message_model_.remove(id); // 离线消息移除
            }
            // 查询该用户的好友信息并返回
            vector<User> user_vec = friend_model_.query(id);
            if (!user_vec.empty())
            {
                vector<string> vec2;
                for (User &user : user_vec)
                {
                    json js;
                    js["id"] = user.GetId();
                    js["name"] = user.GetName();
                    js["state"] = user.GetState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }
            // 查询用户的群组信息
            vector<Group> groupuserVec = group_model_.QueryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.GetId();
                    grpjson["groupname"] = group.GetName();
                    grpjson["groupdesc"] = group.GetDesc();
                    vector<string> userV;
                    for (GroupUser &user : group.GetUsers())
                    {
                        json js;
                        js["id"] = user.GetId();
                        js["name"] = user.GetName();
                        js["state"] = user.GetState();
                        js["role"] = user.GetRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
                response["groups"] = groupV;
                conn->send(response.dump()); // 返回客户端
            }
        }
    }
    else
    {
        json response; // 登陆失败
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;                            // 如果为-1表示注册失败
        response["errmsg"] = "id or password is invalid"; // 如果为-1表示注册失败
        conn->send(response.dump());                      // 返回客户端
    }
}
// 处理注册业务，  name password
// 业务层都是对象  网络，业务，数据模块解耦
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.SetName(name);
    user.SetPassWord(pwd);
    bool state = user_model_.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0; // 如果为-1表示注册失败
        response["id"] = user.GetId();
        conn->send(response.dump()); // 返回客户端
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;       // 如果为-1表示注册失败
        conn->send(response.dump()); // 返回客户端
    }
}
void ChatService::Onchat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
   
   // 如果在该服务器直接发送
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> lock(conn_mutex_);
        auto iter = user_conn_map_.find(toid);
        if (iter != user_conn_map_.end())
        {
            // todo在线，转发消息        服务器主动推送消息给toid用户
            iter->second->send(js.dump()); // a给b 发，
            return;
        }
    }

    // 如果在其他服务器，通过redis中间件发送 ，每一个登录的用户都会订阅该用户的id信息
   // 查询toid是否在线 
    User user = user_model_.query(toid);
    if (user.GetState() == "online")
    {   //发布的过程中，用户可能断开离线，所以回调时，需要再判断
        redis_.publish(toid, js.dump());    //给其他服务器发消息，其他服务器订阅
        return;
    }
    // todo不在线，存储离线消息
    offline_message_model_.insert(toid, js.dump()); // js.dump json 转 string
}

void ChatService::AddFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 添加好友业务  msgid friendid
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();
    friend_model_.insert(userid, friendid); // 存储好友信息
}

void ChatService::CreateGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (group_model_.CreateGroup(group))
    {
        // 存储群组创建人信息
        group_model_.AddGroup(userid, group.GetId(), "creator");
    }
}

void ChatService::AddGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    group_model_.AddGroup(userid, groupid, "normal");
}

void ChatService::GroupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = group_model_.QueryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(conn_mutex_);
    for (int id : useridVec)
    {
        auto it = user_conn_map_.find(id); // 在线
        if (it != user_conn_map_.end())
        {
            // 转发群消息
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线
            User user = user_model_.query(id);
            if (user.GetState() == "online")
            {
                 redis_.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                offline_message_model_.insert(id, js.dump());
            }
        }
    }
}

//
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(conn_mutex_);
        auto it = user_conn_map_.find(userid);
        if (it != user_conn_map_.end())
        {
            user_conn_map_.erase(it);
        }
    }

    //用户注销，相当于就是下线，在redis中取消订阅通道
   redis_.unsubscribe(userid); 

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    user_model_.UpdateState(user);

}

// 处理客户端异常退出
void ChatService::ClientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(conn_mutex_);
        for (auto it = user_conn_map_.begin(); it != user_conn_map_.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.SetId(it->first);
                user_conn_map_.erase(it);
                break;
            }
        }
    }

    //用户注销，相当于就是下线，在redis中取消订阅通道
   redis_.unsubscribe(user.GetId()); 

    // 更新用户的状态信息
    if (user.GetId() != -1)
    {
        user.SetState("offline");
         user_model_.UpdateState(user);
    }
}
