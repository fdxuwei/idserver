#include <iostream>
#include <map>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "Connection.h"
#include "ConnectionPool2.h"
#include "ConnectionPoolMgr.h"


using namespace std;
using namespace cppmysql;

#define MAX_SQL_LEN 1024

class TestConnection
{
public:
	TestConnection()
		: conn_ ("127.0.0.1", 3306, "root", "123456jpns")
		, index_ (0)
	{}
	~TestConnection()
	{
		conn_.close();
	}
	void Run()
	{
		// initialize 
		if(!conn_.connect())
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		conn_.selectDB("test");
		if(!conn_.isTableExist("stu"))
		{
			// create table
			bool ret = conn_.query("create table stu(fno int primary key, fname varchar(20), fage int, fscore float)");
			if(!ret)
			{
				cout << conn_.getErrorStr() << endl;
				assert(false);
			}
		}
		// clear all existing data to prepare for the following test
		bool ret = conn_.query("delete from stu");
		if(!ret)
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}		
		// test case
		TestInsert();
		TestSelect();		
		TestSelectStore();
		TestTransaction();
		TestSelect();
		TestNoTransaction();
		TestSelect();
		TestNoSqlInsert();
		TestSelect();
		TestInsertOrUpdate();
		TestSelect();
		TestNoSqlUpdate();
		TestSelect();
		TestResultSetInSingleThread();
		TestResultSetInMultiThread();
		// close
		conn_.close();
			
	}
private:

	void TestSelect()
	{
		cout << "*******************TestSelect*********************" << endl;
		// query
		if(!conn_.query("select * from stu"))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		// get resultset
		ResultSetPtr rsp = conn_.getResultSet(false);
		if(!rsp)
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}		
		// get all data
		while(rsp->moveNext())
		{
			cout << rsp->getInteger("fno") << ", "
				<< rsp->getString("fname") << ", "
				<< rsp->getInteger("fage") << ", "
				<< rsp->getDouble("fscore") << ", "	<< endl;			
		}
	}	

	void TestSelectStore()
	{
		cout << "*******************TestSelectStore*********************" << endl;
		// query
		if(!conn_.query("select * from stu"))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		// get resultset
		ResultSetPtr rsp = conn_.getResultSet(true);
		if(!rsp)
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}		
		// get all data
		while(rsp->moveNext())
		{
			cout << rsp->getInteger("fno") << ", "
				<< rsp->getString("fname") << ", "
				<< rsp->getInteger("fage") << ", "
				<< rsp->getDouble("fscore") << ", "	<< endl;			
		}
	}


	void TestInsert()
	{
		cout << "*******************TestInsert*********************" << endl;
		char sql[MAX_SQL_LEN];

		for(; index_ < 5; index_++)
		{
			sprintf(sql, "insert into stu(fno,fname,fage,fscore) values(%d,'%s',%d,'%f')", index_, "zhangsan", index_+20, index_+100/1.2);
			if(!conn_.query(sql))
			{
				cout << conn_.getErrorStr() << endl;
				assert(false);				
			}
		}
		// cout << "successfully insert: " << conn_.GetAffectedRows() << endl; // is not the number of rows inserted
	}

	void TestNoSqlInsert()
	{
		cout << "*******************TestNoSqlInsert*********************" << endl;
		Fields flds;
		flds.set("fno", index_++);
		flds.set("fname", "NoSQLInsert");
		flds.set("fage", "30");
		if(!conn_.insert("stu", flds))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
	}

	void TestInsertOrUpdate()
	{
		cout << "*******************TestInsertOrUpdate*********************" << endl;
		Fields fi, fu;
		// real insert
		// fi
		fi.set("fno", index_++);
		fi.set("fname", "InsertOrUpdate1");
		fi.set("fage", "30");
		// fu
		fu.set("fname", "InsertOrUpdate2");
		fu.set("fage", "300");
		if(!conn_.insertOrUpdate("stu", fi, fu))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		TestSelect();
		cout << "insert again: "  << endl;
		// update
		if(!conn_.insertOrUpdate("stu", fi, fu))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		
	}
	
	void TestNoSqlUpdate()
	{
		cout << "*******************TestNoSqlUpdate*********************" << endl;
		Fields fld;
		fld.set("fname", "NoSqlUpdate");
		fld.set("fage", "12345");
		if(!conn_.update("stu", fld, "where fno=0"))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
	}
	
	void TestTransaction()
	{
		cout << "*******************TestTransaction*********************" << endl;
		if(!conn_.setAutoCommit(false))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		char sql[MAX_SQL_LEN];
		// it's better to put an transaction in a try..catch, or use goto
		try
		{
			sprintf(sql, "insert into stu(fno,fname,fage,fscore) values(%d,'transaction',123,'99.9')", index_++);
			if(!conn_.query(sql))
			{
				cout << conn_.getErrorStr() << endl;
				throw 0; // throw an integer to rollback				
			}

			sprintf(sql, "update stu set fname='transaction' where fno=1");
			if(!conn_.query(sql))
			{
				cout << conn_.getErrorStr() << endl;
				throw 0; // throw an integer to rollback				
			}


			// commit if all query is ok
			if(!conn_.commit())
			{
				cout << conn_.getErrorStr() << endl;
				assert(false);
			}
		}
		catch(int)
		{
			// an error occured, rollback
			if(!conn_.rollback())
			{
				cout << conn_.getErrorStr() << endl;
				assert(false);
			}
		}

	}

	// recover from transaction
	void TestNoTransaction()
	{
		cout << "*******************TestNoTransaction*********************" << endl;
		char sql[MAX_SQL_LEN];
		
		if(!conn_.setAutoCommit(true))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		sprintf(sql, "insert into stu(fno,fname,fage,fscore) values(%d,'notransaction',124,'99.9')", index_++);
		if(!conn_.query(sql))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);				
		}
	}

	void TestResultSetInSingleThread()
	{
		cout << "*******************TestResultSetInSingleThread*********************" << endl;
		if(!conn_.select("stu", "*", "where fno <= 5"))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		ResultSetPtr rsp1 = conn_.getResultSet();
		if(!rsp1)
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		if(!conn_.select("stu", "*", "where fno >= 4"))
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}
		ResultSetPtr rsp2 = conn_.getResultSet();
		if(!rsp2)
		{
			cout << conn_.getErrorStr() << endl;
			assert(false);
		}		
		cout << "====================data from resultset 1====================" << endl;
		while(rsp1->moveNext())
		{
			cout << rsp1->getInteger("fno") << ", "
				<< rsp1->getString("fname") << ", "
				<< rsp1->getInteger("fage") << ", "
				<< rsp1->getDouble("fscore") << ", "	<< endl;	
		}
		cout << "====================data from resultset 2====================" << endl;		
		while(rsp2->moveNext())
		{
			cout << rsp2->getInteger("fno") << ", "
				<< rsp2->getString("fname") << ", "
				<< rsp2->getInteger("fage") << ", "
				<< rsp2->getDouble("fscore") << ", "	<< endl;	
		}		
	}

	void TestResultSetInMultiThread()
	{
		cout << "*******************TestResultSetInMultiThread*********************" << endl;
		const int kThreadNum = 4;
		pthread_t threads[kThreadNum];
		for(int i = 0; i < kThreadNum; i++)
		{
			pthread_create(&threads[i], NULL, TestConnection::ThreadHandler, this);
		}

		for(int i = 0; i < kThreadNum; i++)
		{
			pthread_join(threads[i], NULL);
		}

	}	

	static void* ThreadHandler(void *param)
	{
		for(int i = 0; i < 100; i++)
		{
			TestConnection *tc = (TestConnection*)(param);
			Connection &conn = tc->conn_;
			ResultSetPtr rsp;
			{
				LockType lk(tc->mutex_);
				if(!conn.select("stu", "*", NULL))
				{
					cout << conn.getErrorStr() << endl;
					assert(false);
				}
				rsp = conn.getResultSet();
				if(!rsp)
				{
					cout << conn.getErrorStr() << endl;
					assert(false);
				}
			}
			while(rsp->moveNext())
			{
				//do nothing
			}		
		}
		
		return 0;
	}

	Connection conn_;
	int index_;
	typedef boost::lock_guard<boost::mutex> LockType;
	boost::mutex mutex_;
};

class TestConnectionPool
{
public:
	TestConnectionPool(): index_ (100){}
	void TestSingleThread()
	{
		cout << "*******************TestSingleThread*********************" << endl;
		char sql[MAX_SQL_LEN];
		// get a connection 
		Connection* conn1 = pool_.getConnection();
		if(!conn1)
		{
			cout << pool_.getErrorStr() << endl;
			assert(false);				
		}
		sprintf(sql, "insert into stu(fno,fname,fage,fscore) values(%d,'conn1',124,'99.9')", index_++);
		if(!conn1->query(sql))
		{
			cout << conn1->getErrorStr() << endl;
			assert(false);	
		}
		// get another connection 
		Connection* conn2 = pool_.getConnection();
		if(!conn2)
		{
			cout << pool_.getErrorStr() << endl;
			assert(false);				
		}
		sprintf(sql, "insert into stu(fno,fname,fage,fscore) values(%d,'conn2',124,'99.9')", index_++);
		if(!conn2->query(sql))

		{
			cout << conn2->getErrorStr() << endl;
			assert(false);	
		}

		pool_.releaseConnection(conn1);
		pool_.releaseConnection(conn2);

	}

	void TestMultiThread()
	{
		cout << "*******************TestMultiThread*********************" << endl;
		const int kThreadNum = 4;
		pthread_t threads[kThreadNum];
		for(int i = 0; i < kThreadNum; i++)
		{
			pthread_create(&threads[i], NULL, TestConnectionPool::ThreadHandler, this);
		}

		for(int i = 0; i < kThreadNum; i++)
		{
			pthread_join(threads[i], NULL);
		}
	}

	static void* ThreadHandler(void *param)
	{
		TestConnectionPool * tcp = (TestConnectionPool*)param;
		char sql[MAX_SQL_LEN];
		// test operation
		Connection* conn1 = tcp->pool_.getConnection();
		if(!conn1)
		{
			cout << tcp->pool_.getErrorStr() << endl;
			assert(false);				 
		}
		printf("connect success\n");
		sprintf(sql, "insert into stu(fno,fname,fage,fscore) values(%d,'thread',124,'99.9')", syscall(SYS_gettid));
		if(!conn1->query(sql))
		{
			cout << conn1->getErrorStr() << endl;
			assert(false);	
		}
		tcp->pool_.releaseConnection(conn1);
		
		//test thread safty
		for(int i = 0; i < 1000000; i++)
		{
			conn1 = tcp->pool_.getConnection();
			tcp->pool_.releaseConnection(conn1);
		}

		return (NULL);
	}
	void Run()
	{
		// initialize connection pool
		if(!pool_.init("127.0.0.1", 3306, "root", "123456jpns", "test",  4, 20))
		{
			cout << pool_.getErrorStr() << endl;
			assert(false);				
		}
	
		// test case
//		TestSingleThread();
		TestMultiThread();
		//
		pool_.destroy();
	}
private:
	int index_;
	ConnectionPool pool_;
		
};

class TestConnectionPoolMgr
{
public:
	void Run()
	{
		cout << "*******************TestConnectionPoolMgr*********************" << endl;
		// initialize ConnectionPoolMgr
		ConnectionPoolMgr &cpm = ConnectionPoolMgr::instance();
		if(!cpm.addConnectionPool("mysql0",  "127.0.0.1", 3306, "root", "123456jpns", "test0",  4, 20))
		{
			cout << cpm.getErrorStr() << endl;
			assert(false);
		}

		if(!cpm.addConnectionPool("mysql1",  "127.0.0.1", 3306, "root", "123456jpns", "test1",  4, 20))
		{
			cout << cpm.getErrorStr() << endl;
			assert(false);
		}
		// clear datas
		ConnectionPool *pool0 = cpm.getConnectionPool("mysql0");
		Connection *conn0 = pool0->getConnection();
		if(!conn0->isTableExist("stu"))
		{
			// create table
			bool ret = conn0->query("create table stu(fno int primary key, fname varchar(20), fage int, fscore float)");
			if(!ret)
			{
				cout << conn0->getErrorStr() << endl;
				assert(false);
			}
		}		
		if(!conn0->query("delete from stu"))
		{
			cout << conn0->getErrorStr() << endl;
			assert(false);
		}
		pool0->releaseConnection(conn0);
		ConnectionPool *pool1 = cpm.getConnectionPool("mysql1");
		Connection *conn1 = pool1->getConnection();
		if(!conn1->isTableExist("stu"))
		{
			// create table
			bool ret = conn1->query("create table stu(fno int primary key, fname varchar(20), fage int, fscore float)");
			if(!ret)
			{
				cout << conn1->getErrorStr() << endl;
				assert(false);
			}
		}		
		if(!conn1->query("delete from stu"))
		{
			cout << conn0->getErrorStr() << endl;
			assert(false);
		}
		pool1->releaseConnection(conn1);
		// test cases
		
		TestMultiThread();
		// destroy
		cpm.destroy();
	}

	void TestMultiThread()
	{
		cout << "*******************TestMultiThread*********************" << endl;
		const int kThreadNum = 4;
		pthread_t threads[kThreadNum];
		const char *mysql[] = {"mysql0", "mysql1"};
		thread_idx_["mysql0"] = 0;
		thread_idx_["mysql1"] = 0;
		for(int i = 0; i < kThreadNum; i++)
		{
			pthread_create(&threads[i], NULL, TestConnectionPoolMgr::ThreadHandler, const_cast<char*>(mysql[i%(sizeof(mysql)/sizeof(mysql[0]))]));
		}

		for(int i = 0; i < kThreadNum; i++)
		{
			pthread_join(threads[i], NULL);
		}		

		
	}
	static void* ThreadHandler(void *param)
	{
		string name = string((const char *)param);
		int tid = syscall(SYS_gettid);
		//test for mysql0
		cout << "thread " << tid << " : " << name << endl;
		for(int i = 0; i < 1000; ++i)
		{
			ConnectionPoolMgr &cpm = ConnectionPoolMgr::instance();
			ConnectionPool *pool = cpm.getConnectionPool(name.c_str());
			assert(pool);
			Connection *conn = pool->getConnection();
			if(NULL == conn)
			{
				cout << "Get connection from pool failed. " << endl;
				return NULL;
			}
			int32_t index = __sync_fetch_and_add(&thread_idx_[name], 1);
			Fields flds;
			flds.set("fno", index);
			flds.set("fname", "thread");
			flds.set("fscore", tid);
			if(!conn->insert("stu", flds))
			{
				cout << conn->getErrorStr() << endl;
				assert(false);
			}
			pool->releaseConnection(conn);
		}
	}
	
private:	
	static map<std::string, int> thread_idx_;
};

map<std::string, int> TestConnectionPoolMgr::thread_idx_;

class TimeCounter
{
public:
	void start()
	{
		gettimeofday(&tv_start_, NULL);
	}
	int getMiliSec()
	{
		timeval tv;
		gettimeofday(&tv, NULL);
		return ((tv.tv_sec-tv_start_.tv_sec)*1000 + (tv.tv_usec-tv_start_.tv_usec)/1000);
	}
private:
	timeval tv_start_;
};


class TestSelectDB
{
public:
	TestSelectDB()
		: index0_ (0)
		, index1_ (0)
		{}
	void Run()
	{
		cout << "**********************TestSelectDB************************" << endl;
		// initialize connection pool
		if(!pool_.init("127.0.0.1", 3306, "root", "123456jpns", "test1",  4, 20))
		{
			cout << pool_.getErrorStr() << endl;
			assert(false);				
		}
		Connection *conn = pool_.getConnection();
		assert(conn->selectDB("test0"));
		if(!conn->isTableExist("stu"))
		{
			// create table
			bool ret = conn->query("create table stu(fno int primary key, fname varchar(20), fage int, fscore float)");
			if(!ret)
			{
				cout << conn->getErrorStr() << endl;
				assert(false);
			}
		}		
		if(!conn->query("delete from stu"))
		{
			cout << conn->getErrorStr() << endl;
			assert(false);
		}
		assert(conn->selectDB("test1"));
		if(!conn->isTableExist("stu"))
		{
			// create table
			bool ret = conn->query("create table stu(fno int primary key, fname varchar(20), fage int, fscore float)");
			if(!ret)
			{
				cout << conn->getErrorStr() << endl;
				assert(false);
			}
		}		
		if(!conn->query("delete from stu"))
		{
			cout << conn->getErrorStr() << endl;
			assert(false);
		}		
		pool_.releaseConnection(conn);
		TestInsert();

	}

	void TestInsert()
	{
		cout << "**********************Test Insert************************" << endl;

		Connection *conn = pool_.getConnection();
		if(conn == NULL)
		{
			cout << pool_.getErrorStr() << endl;
			assert(false);
		}	
		TimeCounter tc;
		// with select
		tc.start();		
		for(int i = 0; i < 1000; i++)
		{
			conn->selectDB("test0");
			Insert(conn, index0_++);
			conn->selectDB("test1");
			Insert(conn, index1_++);
		}
		cout << "time cost(with select) : " << tc.getMiliSec() << "(ms)" << endl;
		
		//without select
		tc.start();
		conn->selectDB("test0");
		for(int i = 0; i < 1000; i++)
		{	
			Insert(conn, index0_++);
		}
		conn->selectDB("test1");
		for(int i = 0; i < 1000; i++)
		{	
			Insert(conn, index1_++);
		}		
		cout << "time cost(without select) : " << tc.getMiliSec() << "(ms)" << endl;		
		pool_.releaseConnection(conn);
	}
	
private:
	void Insert(Connection* conn, int index)
	{

		Fields flds;
		flds.set("fno", index);
		flds.set("fname", "insert");
		flds.set("fage", "30");
		if(!conn->insert("stu", flds))
		{
			cout << conn->getErrorStr() << endl;
			assert(false);
		}
		
	}
	int index0_;	
	int index1_;
	ConnectionPool pool_;
};

int main()
{
	// single connection
//	TestConnection tc;
//	tc.Run();	

	// single connection pool. one mysql instance, one connection pool.
	TestConnectionPool tcp;
	tcp.Run();
	
	//	connection pool manager, multiple mysql instance
//	TestConnectionPoolMgr tcpm;
//	tcpm.Run();

	// test selectdb performance
//	TestSelectDB tsd;
//	tsd.Run();
	return 0;
}
