#include <stdio.h>
#include <endian.h>
#include "IdServerImp.h"
#include "BlockTcpClient.h"

using namespace std;

enum PROTO_TYPE
{
	PROTO_ID_REQ = 1,
	PROTO_ID_RSP = 2,
};

string IdServerImp::ip_("127.0.0.1");
int IdServerImp::port_(8000);
int IdServerImp::bizType_(0);

IdServerImp::IdServerImp()
	: tcpClient_ (new BlockTcpClient(ip_, port_))
	, cacheNum_ (DEFAULT_CACHE_NUM)
	, nextId_ (0)
	, maxId_ (0)
{

}

IdServerImp::~IdServerImp()
{
	delete tcpClient_;
}

void IdServerImp::setCacheNum(int num)
{
	if(num < 1)
	{
		num = 1;
	}
	cacheNum_ = num > MAX_CACHE_NUM ? MAX_CACHE_NUM : num;
}

bool IdServerImp::getId(uint64_t &id)
{
	if(nextId_ < maxId_)
	{
		id = nextId_;
		++nextId_;
	}
	else
	{
		if(!requestId())
		{
			return false;
		}
		if(nextId_ >= maxId_)
		{
			errStr_ = "id exhausted!";
			return false;
		}
		id = nextId_;
		++nextId_;
		return true;
	}
	return true;
}

bool IdServerImp::requestId()
{
	char reqBuf[32];
	int reqLen = 8; // reqLen(2) + protoType(2) + bizType(2) + reqNum(2)
	*(uint16_t*)(reqBuf) = htobe16(static_cast<uint16_t>(reqLen));
	*(uint16_t*)(reqBuf+2) = htobe16(static_cast<uint16_t>(PROTO_ID_REQ));
	*(uint16_t*)(reqBuf+4) = htobe16(static_cast<uint16_t>(bizType_));
	*(uint16_t*)(reqBuf+6) = htobe16(static_cast<uint16_t>(cacheNum_));
	//
	int retryCnt = 0;
	//
START_REQUEST:
	//
	if(!tcpClient_->send(reqBuf, reqLen))
	{
		errStr_ = "send request failed: " + tcpClient_->errstr();
		return false;
	}
	//
	char rcvBuf[32];
	int rcvLen = 20; // reqLen(2) + bizType(2) + nextId(8) + maxId(8)
	if(!tcpClient_->recv(rcvBuf, rcvLen))
	{
		errStr_ = "recv response failed: " + tcpClient_->errstr();
		return false;
	}
	//
	int rspLen = static_cast<int>(be16toh(*(uint16_t*)(rcvBuf)));
	int protoType = static_cast<int>(be16toh(*(uint16_t*)(rcvBuf+2)));
	//
	if(rspLen != rcvLen || protoType != PROTO_ID_RSP)
	{
		tcpClient_->close();
		if(retryCnt < 2) // retry only once
		{
			++retryCnt;
			goto START_REQUEST;
		}
		else
		{
			char buf[128];
			snprintf(buf, sizeof(buf), "wrong packet: len=%d, type=%d", rspLen, protoType);
			errStr_ = buf;
			return false;
		}
	}
	//
	nextId_ = be64toh(*(uint64_t*)(rcvBuf+4));
	maxId_ = be64toh(*(uint64_t*)(rcvBuf+12));
	//
	return true;
}