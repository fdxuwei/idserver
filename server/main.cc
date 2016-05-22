#include <iostream>
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include "IdServer.h"
#include "IdCache.h"
#include "Config.h"
#include "Daemon.h"
//#include "InitLog.h"

using namespace std;
using namespace muduo;
using namespace muduo::net;

Config gConfig;
InitLog initLog;

int main()
{
	//
	if(!gConfig.load())
	{
		cout << "load config failed." << endl;
		return 1;
	}
	if(gConfig.isDaemon_ && (!setDaemon()))
	{
		cout << "set daemon failed." << endl;
		return 2;
	}
	//
//	initLog.init("idserver", "./", gConfig.logLevel_, 50*1000*1000);
	//
	IdCache::setDbInfo(gConfig.dbIp_, gConfig.dbPort_, gConfig.dbUsr_, gConfig.dbPwd_);
	//
	EventLoop loop;
	IdServer server(&loop, InetAddress(gConfig.listenIp_, gConfig.listenPort_));
	loop.loop();
	//
	return 0;
}