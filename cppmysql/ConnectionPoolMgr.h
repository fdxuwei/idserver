#ifndef _CPPMYSQL_CONNECTIONPOOLMGR_H
#define _CPPMYSQL_CONNECTIONPOOLMGR_H

#include <string>
#include <map>
//#include "ConnectionPool.h"
#include "ConnectionPool.h"
#include "Connection.h"
#include "Error.h"

namespace cppmysql
{
//

class ConnectionPoolMgr
{
public:
	// singleton
	static ConnectionPoolMgr& instance()
	{
		static ConnectionPoolMgr cpm;
		return cpm;
	}
	~ConnectionPoolMgr();
	// get connection pool
	bool isPoolExist(const char *name) const;
	ConnectionPool* getConnectionPool(const char *name);
	bool addConnectionPool(const char *name, 
								const char *host, 
								unsigned short port, 
								const char *usr,
								const char *pwd,
								const char *db, /* this can be NULL */
								int min_pool_size,
								int max_pool_size);
	void destroy();
	const char *  getErrorStr(){return error_.getErrorStr(); }
private:
	ConnectionPoolMgr();
	typedef std::map<std::string, ConnectionPool*> PoolsType;
	PoolsType pools_;
	Error error_;
};

}

#endif

