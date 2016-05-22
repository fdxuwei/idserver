#ifndef __CPPMYSQL_ERROR_H_
#define __CPPMYSQL_ERROR_H_

#include <string>
#include <map>
#include <pthread.h>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

namespace cppmysql
{
// thread safty error
class Error
{
public:
	const char *getErrorStr() const;
	void set(const char *err);
private:
	typedef boost::shared_mutex MutexType;
	typedef boost::shared_lock<MutexType> ReadLock;
	typedef boost::lock_guard<MutexType> WriteLock;
	typedef std::map<int, std::string> ThreadErrorType;
	ThreadErrorType thread_error_;
	mutable MutexType mutex_;
};

}

#endif