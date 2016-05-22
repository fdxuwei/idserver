#ifndef _ID_CACHE_H_
#define _ID_CACHE_H_

#include <map>
#include <boost/shared_ptr.hpp>
#include <cppmysql/Connection.h>
#include "Types.h"

class IdCache
{
public:
	static void setDbInfo(const std::string &dbip, int dbport, const std::string &dbusr, const std::string &dbpwd);
	CachedIds getId(int bizType, int num);
private:

	//
	void refillCachedIds(CachedIds &ids, int bizType);
	bool resetConn();
	//
	static std::string dbip_;
	static int dbport_;
	static std::string dbusr_;
	static std::string dbpwd_;
	//
	typedef std::map<int, CachedIds> CachedIdsMap;
	CachedIdsMap cachedIds_;
	typedef boost::shared_ptr<cppmysql::Connection> ConnPtr;
	ConnPtr conn_;
};

#endif