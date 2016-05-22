#include <set>
#include <iostream>
#include <assert.h>
#include <stdint.h>
#include <sys/time.h>
#include "IdServer.h"

using namespace std;
using namespace idserver;

class TimeCounter
{
public:
	TimeCounter()
	{
		reset();
	}
	void reset()
	{
		gettimeofday(&tv_, NULL);
	}
	int getMiliSec()
	{
		struct timeval tv2;
		gettimeofday(&tv2, NULL);
		return (tv2.tv_sec-tv_.tv_sec)*1000 - (tv2.tv_usec-tv_.tv_usec)/1000;
	}
private:
	struct timeval tv_;
};

void testSingleThread()
{
	set<uint64_t> checkUnique;
	TimeCounter tc;
	int reqNum = 1000000;
	cout << "******test for single thread*****" << endl;
	for(int i = 0; i < reqNum; ++i)
	{
		uint64_t id;
		if(getId(id))
		{
			// check unique
			pair<set<uint64_t>::iterator, bool> p = checkUnique.insert(id);
			assert(p.second);
		}
		else
		{
			cout << "get id failed: " << errStr() << endl;
			break;
		}
	}
	//
	cout << "request " << reqNum << ", time costs: " << tc.getMiliSec() << " ms" << endl;

}

void* threadFun(void*);
void testMultiThread()
{
	int threadNum = 10;
	pthread_t t[threadNum];
	set<uint64_t> sets[threadNum];
	for(int i = 0; i < threadNum; ++i)
	{
		pthread_create(&t[i], NULL, &threadFun, &sets[i]);
	}
	for(int i = 0; i < threadNum; ++i)
	{
		pthread_join(t[i], NULL);
	}
	// check unique
	for(int i = 1; i < threadNum; ++i)
	{
		sets[0].insert(sets[i].begin(), sets[i].end());
	}
	assert(sets[0].size() == threadNum*1000000);
}

void* threadFun(void* param)
{
	TimeCounter tc;
	int reqNum = 1000000;
	cout << "******test for multi-thread******" << endl;
	set<uint64_t> *su = (set<uint64_t> *)param;
	for(int i = 0; i < reqNum; ++i)
	{
		uint64_t id;
		if(!getId(id))
		{
			cout << "get id failed: " << errStr() << endl;
			break;
		}
		su->insert(id);
	}
	//
	cout << "time costs: " << tc.getMiliSec() << " ms" << endl;
	return 0;
}

int main()
{
	init("127.0.0.1", 8000, 0);
	//
	//testSingleThread();
	//
	testMultiThread();
}