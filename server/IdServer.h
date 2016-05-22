#ifndef IDSERVER_H
#define IDSERVER_H

#include <muduo/base/Timestamp.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/InetAddress.h>

class IdServer
{
public:
	IdServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr);
private:
	void onConnection(const muduo::net::TcpConnectionPtr &conn);
	void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time);
	void handleIdRequest(const muduo::net::TcpConnectionPtr &conn, const char *data, int len);
	muduo::net::TcpServer server_;
};

#endif