#include <stdint.h>
#include <boost/bind.hpp>
#include <muduo/base/Logging.h>
#include <muduo/net/Endian.h>
#include "IdServer.h"
#include "ThreadObject.h"
#include "IdCache.h"

using namespace std;
using namespace muduo;
using namespace muduo::net;

#define BASE_HEAD_LEN 4

enum PROTO_TYPE
{
	PROTO_ID_REQ = 1,
	PROTO_ID_RSP = 2,
};

IdServer::IdServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr)
	: server_ (loop, listenAddr, "IdServer")
{
	server_.setConnectionCallback(boost::bind(&IdServer::onConnection, this, _1));
	server_.setMessageCallback(boost::bind(&IdServer::onMessage, this, _1, _2, _3));
	server_.start();
}

void IdServer::onConnection(const muduo::net::TcpConnectionPtr &conn)
{

}

void IdServer::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time)
{
	int16_t packetLen;
	int16_t protoType;
	int readableBytes;
	//
	while(((readableBytes = buf->readableBytes()) >= BASE_HEAD_LEN)
		&& ((packetLen = buf->peekInt16()) <= readableBytes))
	{
		if(packetLen < 0)
		{
			LOG_ERROR << "Invalid packet len: " << packetLen;
			conn->forceClose();
			return;
		}
		buf->readInt16();
		protoType = buf->readInt16();
		if(1 != protoType)
		{
			LOG_ERROR << "Invalid protocol :" << protoType;
			conn->forceClose();
			return;
		}
		//
		int dataLen = packetLen-BASE_HEAD_LEN;
		handleIdRequest(conn, buf->peek(), dataLen);
		//
		buf->retrieve(dataLen);
	}
}

void IdServer::handleIdRequest(const muduo::net::TcpConnectionPtr &conn, const char *data, int len)
{
	using namespace muduo::net::sockets;
	//
	if(len != 4)
	{
		LOG_ERROR << "invalid data len: " << len;
		conn->forceClose();
		return;
	}
	int bizType = static_cast<int>(networkToHost16(*(uint16_t*)data));
	int reqNum = static_cast<int>(networkToHost16(*(uint16_t*)(data+2)));
	IdCache *ic = ThreadObject<IdCache>::getInstance();
	CachedIds ids = ic->getId(bizType, reqNum);
	//
	char buf[32];
	char *tmp = buf;
	int packlen = BASE_HEAD_LEN + sizeof(IdType)*2;
	*(uint16_t*)tmp = hostToNetwork16(packlen);
	tmp += sizeof(uint16_t);
	*(uint16_t*)tmp = hostToNetwork16(PROTO_ID_RSP);
	tmp += sizeof(uint16_t);
	*(uint64_t*)tmp = hostToNetwork64(ids.nextId_);
	tmp += sizeof(uint64_t);
	*(uint64_t*)tmp = hostToNetwork64(ids.maxId_);
	tmp += sizeof(uint64_t);
	//
	conn->send(buf, packlen);
}