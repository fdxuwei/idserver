#include "ConnectionPoolMgr.h"

using namespace std;

namespace cppmysql
{

ConnectionPoolMgr::ConnectionPoolMgr()
{}


ConnectionPoolMgr::~ConnectionPoolMgr()
{
	destroy();
}

void ConnectionPoolMgr::destroy()
{
	for(PoolsType::iterator it = pools_.begin(); it != pools_.end(); ++it)
	{
		delete it->second;
	}
	pools_.clear();
}

ConnectionPool* ConnectionPoolMgr::getConnectionPool(const char * name)
{
	PoolsType::iterator it = pools_.find(name);
	if(pools_.end() == it)
	{
		error_.set("Pool not exist.");
		return NULL;
	}
	return it->second;
}

bool ConnectionPoolMgr::isPoolExist(const char *name) const
{
	PoolsType::const_iterator it = pools_.find(name);
	return (pools_.end() != it);
}

bool ConnectionPoolMgr::addConnectionPool(const char * name,
											const char * host,
											unsigned short port,
											const char * usr,
											const char * pwd,
											const char * db,
											int min_pool_size,
											int max_pool_size)
{
	if(name == NULL)
	{
		error_.set("Pool must have a name.");
		return false;
	}
	if(isPoolExist(name))
	{
		error_.set("Pool name already exists.");
		return false;
	}
	ConnectionPool *pool = new ConnectionPool();
	if(!pool->init(host, port, usr, pwd, db, min_pool_size, max_pool_size))
	{
		string errstr = "Initialize connection pool failed, ";
		errstr += pool->getErrorStr();
		error_.set(errstr.c_str());
		delete pool;
		pool = NULL;
		return false;
	}
	pools_[name] = pool;
	return true;
}

}
