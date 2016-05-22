#ifndef THREAD_OBJECT_H
#define THREAD_OBJECT_H

#include <pthread.h>
#include <muduo/base/Logging.h>

// class for thread specific object
template<class Object>
class ThreadObject
{
public:
	static Object *getInstance();
private:
	static void createKey();
	static bool init();
	static void release(void *param = NULL);
	static pthread_key_t key_;
	static pthread_once_t once_;

};

template<class Object>
bool ThreadObject<Object>::init()
{
	using namespace muduo; 
	int ret;
	if((ret = pthread_once(&once_, createKey)))	
	{
		LOG_ERROR << "errno:[" << ret << "]---errMsg:[" << strerror_tl(ret) << "]";
		return false;
	}
	return true;
}



template<class Object>
void ThreadObject<Object>::createKey()
{
	using namespace muduo;
	int ret;
	if((ret = pthread_key_create(&key_, &release)))
	{
		LOG_FATAL << "errno:[" << ret << "]--errMsg:[" << strerror_tl(ret) << "]";
	}
}

template<class Object>
Object* ThreadObject<Object>::getInstance()
{
	using namespace muduo;
	if(!init())
	{
		LOG_ERROR << "Initialize ThreadObject failed.";
		return NULL;
	}
	Object* obj = NULL;
	if((obj = static_cast<Object *>(pthread_getspecific(key_))) == NULL)
	{
		obj = new Object;
		int ret;
		if((ret = pthread_setspecific(key_, obj)))
		{
			LOG_FATAL << "errno:[" << ret << "--errMsg:[" << strerror_tl(ret) << "]";
			delete obj;
			return NULL;
		}
	}
	return obj;
}

template<class Object>
void ThreadObject<Object>::release(void *param)
{
	using namespace muduo;
	Object* obj = (Object*)param;
	if(obj != NULL)
	{
		delete obj;
		LOG_TRACE << "delete obj.";
	}
	LOG_TRACE << "thread specific data released.";
}


template<class Object>
pthread_key_t ThreadObject<Object>::key_;

template<class Object>
pthread_once_t ThreadObject<Object>::once_ = PTHREAD_ONCE_INIT;



#endif