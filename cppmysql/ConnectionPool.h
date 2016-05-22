#ifndef CPPMYSQL_CONNECTIONPOOL2_H
#define CPPMYSQL_CONNECTIONPOOL2_H

#include <string>
#include <deque>
#include <vector>
#include <map>
#include <boost/thread/shared_mutex.hpp>
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
	Connection* allocateConnection();
	// thread safty
	typedef boost::shared_mutex MutexType;
	typedef boost::shared_lock<MutexType> ReadLock;
	typedef boost::lock_guard<MutexType> WriteLock;
	MutexType mutex_;
	// connection
	typedef std::map<int, Connection*> ThreadConnsType;
	ThreadConnsType thread_conns_;
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
