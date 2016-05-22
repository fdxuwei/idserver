#include <unistd.h>
#include <sys/syscall.h>
#include "Error.h"

using namespace std;

namespace cppmysql
{
const char *Error::getErrorStr() const
{
	int tid = syscall(SYS_gettid);
	ReadLock rl(mutex_);
	ThreadErrorType::const_iterator it = thread_error_.find(tid);
	if(thread_error_.end() == it)
		return NULL;
	else
		return it->second.c_str();
}

void Error::set(const char *err)
{
	int tid = syscall(SYS_gettid);
	ThreadErrorType::iterator it;
	{
		ReadLock rl(mutex_);
		it = thread_error_.find(tid);
		if(thread_error_.end() != it)
		{
			//  different thread would modify different key, so a read lock is enough.
			it->second = string(err); 
		}
	}
	if(thread_error_.end() == it)
	{
		// the first error of this thread
		WriteLock wl(mutex_);
		thread_error_[tid] = string(err);
	}
}

}