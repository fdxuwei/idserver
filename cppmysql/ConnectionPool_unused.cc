#include <stdio.h>
#include "ConnectionPool.h"

using namespace std;

namespace cppmysql
{

ConnectionPool::ConnectionPool()
	: max_pool_size_ (20)
	, min_pool_size_ (1)	
	, pool_size_ (min_pool_size_)
{
}

bool ConnectionPool::init(const char *host, 
		unsigned short port, 
		const char *usr, 
		const char *pwd, 
		const char *db,
		int min_pool_size,
		int max_pool_size)
{
	host_ = host;
	port_ = port;
	usr_ = usr;
	pwd_ = pwd;
	if(db) db_ = db;
	min_pool_size_ = min_pool_size;
	max_pool_size_ = max_pool_size;
	//
	pool_size_ = min_pool_size_;
	//

	for(int i = 0; i < pool_size_; i++)
	{
		Connection *conn = allocateConnection();
		if(NULL == conn)
			return false;
		conns_free_.push_back(conn);
		conns_status_[conn] = CONN_FREE;
	}
	
	return true;
}

Connection* ConnectionPool::getConnection()
{
	Connection* ret = NULL;
	{
		LockType lk(mutex_);
		if(!conns_free_.empty())
		{
			ret = conns_free_.front();
			conns_free_.pop_front();
			conns_status_[ret] = CONN_INUSE;
		}
		else if(pool_size_ < max_pool_size_)
		{
			// allocate a new one
			ret = allocateConnection();
			if(ret)
			{
				conns_status_[ret] = CONN_INUSE;
				++pool_size_;
			}	
		}
		else
		{
			char buf[100];
			sprintf(buf, "No free connections, max_pool_size=%d", max_pool_size_);
			error_.set(buf);
		}
	}

	return ret;
}

void ConnectionPool::releaseConnection(Connection *conn)
{
	ConnsStatusType::iterator it = conns_status_.find(conn);
	assert(conns_status_.end() != it);
	{
		LockType lk(mutex_);
		it->second = CONN_FREE;
		conns_free_.push_back(conn);
	}
}

Connection* ConnectionPool::allocateConnection()
{
	const char *db = db_.empty() ? NULL : db_.c_str();
	Connection* cp = new Connection(host_.c_str(), port_, usr_.c_str(), pwd_.c_str(), db);  
	
	if(!cp->connect())
	{
		string errstr = "Connecting failed, ";
		errstr.append(cp->getErrorStr());
		error_.set(errstr.c_str());
		
		delete cp;
		cp = NULL;
		return NULL;
	}
	
	return cp;
}
void ConnectionPool::destroy()
{
	for(ConnsStatusType::iterator it = conns_status_.begin(); it != conns_status_.end(); ++it)
	{
		// be sure all connections have been return back to pool
		assert(it->second == 0);
		delete it->first;
	}
	conns_status_.clear();
	conns_free_.clear();
}

ConnectionPool::~ConnectionPool()
{
	destroy();
}

} /* cppmysql */