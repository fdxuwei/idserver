#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include "Config.h"

using namespace std;

template<class DataType>
static void getMapValue(const map<string,string> &m, const std::string &key, DataType &val)
{
	map<string,string>::const_iterator it = m.find(key);
	if(m.end() == it)
	{
		throw key;
		return;
	}
	stringstream ss(it->second);
	ss >> val;
}

bool Config::load()
{
	string filename = "idserver.cnf";
	ifstream in(filename.c_str());
	if(!in.good())
	{
		cout << "open file " << filename << " failed: " << strerror(errno) << endl;
		return false;
	}
	//
	//
	map<string,string> cfgMap;
	string key;
	string value;
	while(in)
	{
		in >> key >> value;
		cfgMap[key] = value;
	}
	//
	try
	{
		getMapValue(cfgMap, "is_daemon", isDaemon_);
		getMapValue(cfgMap, "log_level", logLevel_);
		getMapValue(cfgMap, "listen_ip", listenIp_);
		getMapValue(cfgMap, "listen_port", listenPort_);
		getMapValue(cfgMap, "db_ip", dbIp_);
		getMapValue(cfgMap, "db_port", dbPort_);
		getMapValue(cfgMap, "db_usr", dbUsr_);
		getMapValue(cfgMap, "db_pwd", dbPwd_);
	}
	catch(const std::string &key)
	{
		cout << "can not find config of " << key << endl;
		return false;
	}
	return true;
}