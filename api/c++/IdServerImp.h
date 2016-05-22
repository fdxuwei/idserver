#ifndef IDSERVERIMP_H
#define IDSERVERIMP_H

#include <stdint.h>
#include <string>

#define MAX_CACHE_NUM 1000
#define DEFAULT_CACHE_NUM 100

class BlockTcpClient;

class IdServerImp
{
public:
	IdServerImp();
	~IdServerImp();
	bool getId(uint64_t &id);
	void setCacheNum(int num);
	const std::string &errStr() const { return errStr_; }
	//
	static std::string ip_;
	static int port_;
	static int bizType_;
private:
	//
	bool requestId();
	//
	BlockTcpClient *tcpClient_;
	int cacheNum_;
	uint64_t nextId_;
	uint64_t maxId_;
	std::string errStr_;
};

#endif