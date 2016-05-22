#ifndef _DAO_MYSQL_CONNECTIONPTR_H_
#define _DAO_MYSQL_CONNECTIONPTR_H_

#include <assert.h>
#include <cppmysql/Connection.h>
#include <cppmysql/ConnectionPool.h>

namespace dao
{
namespace mysql
{
// helper class
struct ConnectionPtrRef
{
	cppmysql::Connection *conn_;
	cppmysql::ConnectionPool *pool_;
	ConnectionPtrRef(cppmysql::Connection* conn, cppmysql::ConnectionPool *pool)
		: conn_ (conn)
		, pool_ (pool)
	{}
};

class ConnectionPtr
{
public:
	ConnectionPtr()
		: conn_ (NULL)
		, pool_ (NULL)
		{}	
	ConnectionPtr(cppmysql::Connection* conn, cppmysql::ConnectionPool *pool)
		: conn_ (conn)
		, pool_ (pool)
	{}	
	~ConnectionPtr()
	{
//		if(pool_ && conn_)
//			pool_->releaseConnection(conn_);
	}

	cppmysql::Connection* operator->()
	{
		return conn_;
	}
	operator bool ()
	{
		return (conn_ != NULL);
	}

	/**
	  *  The following functions allow constructs such as:
	  *  ConnectionPtr function_returning_ConnectionPtr(){...}
	  */

/*	ConnectionPtr(ConnectionPtrRef ref)
		: conn_ (ref.conn_)
		, pool_ (ref.pool_)	
	{
	}

	ConnectionPtr& operator=(ConnectionPtrRef ref)
	{
		if(ref.conn_ != conn_ && ref.pool_ != pool_)
		{
			conn_ = ref.conn_;
			pool_ = ref.pool_;
		}
		return *this;
	}		

	operator ConnectionPtrRef()
	{
		ConnectionPtrRef ref(conn_, pool_);
		conn_ = NULL;
		pool_ = NULL;
		return ref;
	}
*/	
	
protected:

/*
	ConnectionPtr(ConnectionPtr& cp)
	{
		swap(cp);
	}

	ConnectionPtr& operator=(ConnectionPtr& cp)
	{
		swap(cp);
		return *this;
	}
*/	
	
private:
	void swap(ConnectionPtr& cp)
	{
		if(this != &cp)
		{
			conn_ = cp.conn_;
			pool_ = cp.pool_;
			cp.conn_ = NULL;
			cp.pool_ = NULL;
		}
	}
	cppmysql::Connection *conn_;
	cppmysql::ConnectionPool *pool_;
};
}
}

#endif