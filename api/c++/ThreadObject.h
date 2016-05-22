#ifndef THREAD_OBJECT_H
#define THREAD_OBJECT_H

#include <stdlib.h>
#include <pthread.h>

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
	int ret;
	if((ret = pthread_once(&once_, createKey)))	
	{
		abort();
		return false;
	}
	return true;
}



template<class Object>
void ThreadObject<Object>::createKey()
{
	int ret;
	if((ret = pthread_key_create(&key_, &release)))
	{
		abort();
	}
}

template<class Object>
Object* ThreadObject<Object>::getInstance()
{
	if(!init())
	{
		abort();
		return NULL;
	}
	Object* obj = NULL;
	if((obj = static_cast<Object *>(pthread_getspecific(key_))) == NULL)
	{
		obj = new Object;
		int ret;
		if((ret = pthread_setspecific(key_, obj)))
		{
			abort();
			delete obj;
			return NULL;
		}
	}
	return obj;
}

template<class Object>
void ThreadObject<Object>::release(void *param)
{
	Object* obj = (Object*)param;
	if(obj != NULL)
	{
		delete obj;
	}
}


template<class Object>
pthread_key_t ThreadObject<Object>::key_;

template<class Object>
pthread_once_t ThreadObject<Object>::once_ = PTHREAD_ONCE_INIT;



#endif