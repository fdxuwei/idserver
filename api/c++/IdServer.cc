#include "IdServer.h"
#include "IdServerImp.h"
#include "ThreadObject.h"

using namespace std;

namespace idserver
{
	//
	void init(const std::string &ip, int port, int bizType)
	{
		IdServerImp::ip_ = ip;
		IdServerImp::port_ = port;
	}
	//
	bool getId(uint64_t &id)
	{
		IdServerImp *isp = ThreadObject<IdServerImp>::getInstance();
		return isp->getId(id);
	}
	//
	void setCacheNum(int num)
	{
		IdServerImp *isp = ThreadObject<IdServerImp>::getInstance();
		isp->setCacheNum(num);
	}
	//
	const std::string & errStr()
	{
		IdServerImp *isp = ThreadObject<IdServerImp>::getInstance();
		return isp->errStr();
	}

}

