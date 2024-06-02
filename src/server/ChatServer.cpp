#include "ChatServer.hpp"
#include <functional>
#include <string>
#include "json.hpp"
#include "ChatService.hpp"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;
ChatServer::ChatServer(EventLoop *loop,               // 事件循环
                       const InetAddress &listenAddr, // IP+Port
                       const string &nameArg) : server_(loop, listenAddr, nameArg), loop_(loop)
{
    // 注册链接回调
    server_.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    // 注册消息回调
    server_.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量    1个主要的reactor 3个subreactor 工作线程
    server_.setThreadNum(4);
}

ChatServer::~ChatServer()
{
}

// 启动服务
void ChatServer::start()
{
    server_.start();
}

// 上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown(); // 释放socketfd资源
    }
}
// 默认有this指针
// 处理用户的读写事件
//不同的工作线程
void ChatServer::onMessage(const TcpConnectionPtr &conn, // 连接
                           Buffer *buffer,               // 缓冲区
                           Timestamp time)
{
    //最开始的网络
    string buf = buffer->retrieveAllAsString(); // 从缓冲区中拿数据
    json js = json::parse(buf);                 // 数据的反序列化
    //达到的目的，完全解耦网络模块的代码和业务模块的代码
    //js["msgid"] 获取业务hander  
    //从网络上拿到 msgid 获取相对于的事件处理 没有业务层的方法调用 login reg等方法通过map 
    //业务改动，这里不需要改动
    auto msg_handler = ChatService::GetInstance()->GetHandler(js["msgid"].get<int>()); 
    //回调消息绑定号的事件处理器 
    msg_handler(conn,js,time);      
} 