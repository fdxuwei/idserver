#ifndef CPPMYSQL_CONNECTION_H
#define CPPMYSQL_CONNECTION_H

#include <string>
#include <mysql/mysql.h>
#include <boost/shared_ptr.hpp>
#include "ResultSet.h"
#include "Fields.h"

namespace cppmysql
{

// mysql connection class
class Connection
{
public:
	Connection(const char *host, 
		unsigned short port,
		const char *usr,
		const char *pwd,
		const char *db = NULL);
	~Connection();
	bool connect();
	void close();
	bool selectDB(const char *db);
	// sql operation
	bool query(const char *sql);
	// no sql operation
	bool del(const char *table, const char *condition = NULL); // I prefer to use "delete" as function name here, but it conflict with the operator delete
	bool select(const char *table, const char *fields, const char *condition = NULL);
	bool update(const char *table, const Fields &fields, const char *condition = NULL);
	bool insert(const char *table, const Fields &fields);
	bool insertOrUpdate(const char *table, const Fields &insert_fields, const Fields &update_fields);
	// query result
	ResultSetPtr getResultSet(bool store = false);
	long getAffectedRows();
	const char *getErrorStr() { return errstr_.c_str(); }
	// transaction
	bool setAutoCommit(bool isauto = true);
	bool commit();
	bool rollback();
	// helper functions
	bool isTableExist(const char *table);
private:
	void makeInsertSql(const char *table, const Fields &fields, std::string &sql);
	void makeMysqlError(const std::string &errstr = std::string(""));
	void makeError(const std::string &errstr);
	std::string connectionInfo_;
	std::string host_;
	unsigned short port_;
	std::string usr_;
	std::string pwd_;
	std::string db_;
	std::string errstr_;
	MYSQL *mysql_;
};

//typedef boost::shared_ptr<Connection> ConnectionPtr;

} /*end namespace CppMySql */


#endif
