//moduo网络库
//Tcpserver服务器
//TcpClient客户端
// epoll + 线程池
//网络的I/O代码和业务代码区分开
//用户的连接和断开，用户的可读写事件
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>  //
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo::net;
using namespace muduo;
using namespace placeholders;
/*基于muduo网络库开发服务器程序
1:组合tcpserver对象
2:创建EventLoop事件循环对象的指针 
3:明确tcpserver构造函数参数，输出chatserver
4:服务器类的构造函数中，注册处理连接的回调函数和处理读写事件的回调函数
5：设置合适的服务端线程数量，muduo自己分配io和work线程
*/

class CharServer {
public:
	CharServer(EventLoop* loop,				 //事件循环
		const InetAddress& listenAddr,		 //IP+Port
		const string& nameArg)               //服务器的名称 ，线程名字
		:server_(loop, listenAddr, nameArg),
		loop_(loop) {
		//给服务器注册用户连接的创建和断开回调
		//_1 是参数const TcpConnectionPtr&
		//关注连接的状态
		server_.setConnectionCallback(std::bind(&CharServer::onConnection,this,_1));
		//给服务器注册用户读写事件回调 
		//关注事件读写的状态
		server_.setMessageCallback(std::bind(&CharServer::onMessage, this, _1, _2,_3));
		//设置服务器段的线程数量; 1个i/o 3个work
		server_.setThreadNum(4);
	}
	//开启事件循环
	void start() {
		server_.start();
	}
private:
	//专门处理用户的连接创建和断开 epoll listenfd accpet 
	//成员方法有this指针 这里onConnection会有2个参数，this，conn
	void onConnection(const TcpConnectionPtr& conn) {
		if (conn->connected()) {
			cout << conn->peerAddress().toIpPort() << "-->" << conn->localAddress().toIpPort() << "online" << endl;
		}
		else {
			cout << conn->peerAddress().toIpPort() << "-->" << conn->localAddress().toIpPort() << "offline" << endl;
			conn->shutdown(); //close(fd);   服务端的fd回收
			loop_->quit();    //退出epoll
		}
	}
	//默认有this指针
	//处理用户的读写事件
	void onMessage(const TcpConnectionPtr& conn, //连接
		Buffer* buffer,  //缓冲区
		Timestamp time) { //接收数据的时间信息 
		string buf = buffer->retrieveAllAsString();
		cout << "recv data:" << buf << "time:" << time.toString() << endl;
		conn->send(buf);
	}
	TcpServer server_;
	EventLoop* loop_;     //epoll 
};

int main() {
	EventLoop loop; //epoll
	InetAddress listenAddr("127.0.0.1",6000);		 //IP+Port
	CharServer server(&loop, listenAddr,"ChatServer");
	server.start();  //listenfd epoll_ctl->epoll   添加套接字到epoll
	loop.loop(); //epoll.wait  阻塞的方式等待新用户连接  已连接用户的读写事件	 回调onConnection 和onMessage
	return 0;
}