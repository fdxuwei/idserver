#ifndef CPPMYSQL_CONNECTIONPOOL_H
#define CPPMYSQL_CONNECTIONPOOL_H

#include <string>
#include <deque>
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include "Connection.h"
#include "Error.h"

namespace cppmysql
{

/* ConnectionPool for mysql */
class ConnectionPool
{
public:
	ConnectionPool();
	~ConnectionPool();
	bool init(const char *host, 
		unsigned short port, 
		const char *usr, 
		const char *pwd,
		const char *db,
		int min_pool_size,
		int max_pool_size);
	Connection* getConnection();
	void releaseConnection(Connection*);
	void destroy();
	const char *getErrorStr() {return error_.getErrorStr(); }
private:
	enum CONNSTATUS
	{
		CONN_FREE,
		CONN_INUSE
	};
	Connection* allocateConnection();
	// thread safty
	typedef boost::mutex MutexType;
	typedef boost::lock_guard<MutexType> LockType;
	MutexType mutex_;
	// connection
	typedef std::map<Connection*, int> ConnsStatusType;
	typedef std::deque<Connection*> FreeConnsType;
	ConnsStatusType conns_status_;
	FreeConnsType conns_free_;
	// mysql sql
	std::string host_;
	unsigned short port_;
	std::string usr_;
	std::string pwd_;
	std::string db_;
	// pool
	int min_pool_size_;
	int max_pool_size_;
	int pool_size_;
	//
	Error error_;
};

} /* end namespace CppMySql*/

#endif
