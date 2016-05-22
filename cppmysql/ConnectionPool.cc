#include <stdio.h>
#include <sys/syscall.h>
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
	return true;
}

Connection* ConnectionPool::getConnection()
{
	int tid = syscall(SYS_gettid);
	bool exist = false;
	Connection* ret = NULL;
	{
		ReadLock rl(mutex_);
		ThreadConnsType::iterator itc = thread_conns_.find(tid);
		if(itc != thread_conns_.end())
		{
			exist = true;
			ret = itc->second;
		}
		else
		{
			exist = false;
		}
	}
	if(!exist)
	{
		// allocate a new one
		ret = allocateConnection();
		if(ret)
		{
			WriteLock wl(mutex_);
			thread_conns_[tid] = ret;
		}	
	}

	return ret;
}

void ConnectionPool::releaseConnection(Connection *conn)
{
	// do nothing
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
	for(ThreadConnsType::iterator it = thread_conns_.begin(); it != thread_conns_.end(); ++it)
	{
		delete it->second;
	}
	thread_conns_.clear();
}

ConnectionPool::~ConnectionPool()
{
	destroy();
}

} /* cppmysql */