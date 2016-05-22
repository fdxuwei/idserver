#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "BlockTcpClient.h"

using namespace std;

static __thread char errstrbuf[256];
static const char * syserror()
{
	strerror_r(errno, errstrbuf, sizeof(errstrbuf));
	return errstrbuf;
}

BlockTcpClient::BlockTcpClient(const std::string &ip, int port)
	: ip_ (ip)
	, port_ (port)
	, timeout_ (2)
	, fd_ (-1)
{

}

BlockTcpClient::~BlockTcpClient()
{
	closeFd();
}

void BlockTcpClient::setTimeout(int timeout)
{
	timeout_ = timeout;
}

bool BlockTcpClient::connect()
{
	if(fd_ >= 0)
	{
		// TODO:
		return true;
	}
	//
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(static_cast<unsigned short>(port_));
	sin.sin_addr.s_addr = inet_addr(ip_.c_str());
	fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_ < 0)
	{
		errstr_ = syserror();
		return false;
	}
	//
	struct timeval tv;
	tv.tv_sec = timeout_;
	tv.tv_usec = 0;
	if(setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
	{
		errstr_ = syserror();
		return false;
	}
	if(setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
	{
		errstr_ = syserror();
		return false;
	}

	//
	if(::connect(fd_, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		errstr_ = syserror();
		closeFd();
		return false;
	}
	//
	return true;
}

bool BlockTcpClient::send(const char *data, int size)
{
	if((fd_ < 0) && (!connect()))
	{
		return false;
	}
	//
	if(write(fd_, data, size) != size)
	{
		errstr_ = syserror();
		closeFd();
		return false;
	}
	//
	return true;
}

bool BlockTcpClient::recv(char *data, int size)
{
	if((fd_ < 0) || (!connect()))
	{
		return false;
	}
	//
	int rsize = 0;
	int ret = 0;
	while(rsize < size && (ret = read(fd_, data+rsize, size-rsize)) > 0)
	{
		rsize += ret;
	}
	if(rsize != size)
	{
		if(ret == 0 && errno == 0)
		{
			errstr_ = "connection closed.";
		}
		else
		{
			errstr_ = syserror();
		}
		closeFd();
		return false;
	}
	//
	return true;
}

void BlockTcpClient::closeFd()
{
	if(fd_ >= 0)
	{
		::close(fd_);
	}
	fd_ = -1;
}
