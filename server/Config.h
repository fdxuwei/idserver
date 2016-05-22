#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <sstream>


class Config
{
public:
	bool load();
	//
	bool isDaemon_;
	int logLevel_;
	std::string listenIp_;
	int listenPort_;
	std::string dbIp_;
	int dbPort_;
	std::string dbUsr_;
	std::string dbPwd_;
};

#endif