#pragma once
#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include "UserModel.hpp"
#include "json.hpp"
#include "OfflineMessageModel.hpp"
#include <mutex>
#include "FriendModel.hpp"
#include "redis.hpp"
using json = nlohmann::json;
using namespace std;
using namespace muduo;
using namespace muduo::net;
#include "GroupModel.hpp"
// 处理消息事件回调的方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;
// 聊天服务器业务类
class ChatService
{
public:
    static ChatService *GetInstance();                                   // 获取单例对象
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);  // 登录
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);    // 注册
    void Onchat(const TcpConnectionPtr &conn, json &js, Timestamp time); // 一对一聊天业务

    void AddFriend(const TcpConnectionPtr &conn, json &js, Timestamp time); // 添加好友业务

    void CreateGroup(const TcpConnectionPtr &conn, json &js, Timestamp time); // 创建群组业务

    void AddGroup(const TcpConnectionPtr &conn, json &js, Timestamp time); // 加入群组业务

    void GroupChat(const TcpConnectionPtr &conn, json &js, Timestamp time); // 群组聊天业务

    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time); // 注销业务
    void ClientCloseException(const TcpConnectionPtr &conn);               // 客户端异常退出
    void reset();                                                          // 服务器异常，业务重置方法
    MsgHandler GetHandler(int msg_id);                                     // 获取消息对应的处理器
   
    void handleRedisSubscribeMessage(int, string); // 从redis消息队列中获取订阅的消息

private:
    ChatService();
    mutex conn_mutex_;                                   // 定义互斥锁，保证user_conn_map_的线程安全
    unordered_map<int, MsgHandler> msg_handler_map_;     // 存储消息id和其对应的业务处理方法
    UserModel user_model_;                               // 数据操作类对象
    unordered_map<int, TcpConnectionPtr> user_conn_map_; // 存储在线用户的通信连接 //线程安全
    OfflineMessageModel offline_message_model_;          // 离线消息
    FriendModel friend_model_;                           //
    GroupModel group_model_;                             // 群组相关业务
    Redis redis_;                                        // Redis 操作对象
};