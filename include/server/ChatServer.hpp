#pragma once
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    // 初始化聊天服务器对象
    ChatServer(
        EventLoop *loop,               // 事件循环
        const InetAddress &listenAddr, // IP+Port
        const string &nameArg);
    ~ChatServer();
    // 启动服务
    void start();

private:
    // 专门处理用户的连接创建和断开 epoll listenfd accpet
    // 成员方法有this指针 这里onConnection会有2个参数，this，conn
    void onConnection(const TcpConnectionPtr &conn);
    // 默认有this指针
    // 处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time);
    TcpServer server_; // 组合的muduo库，实现服务器功能的类对象
    EventLoop *loop_;  // 事件循环，合适的时候quit
};
