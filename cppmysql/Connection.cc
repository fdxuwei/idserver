#include <stdio.h>
#include "Connection.h"

using namespace std;

namespace cppmysql
{

Connection::Connection(const char *host, 
		unsigned short port,
		const char *usr,
		const char *pwd,
		const char *db)
	: host_ (host)
	, port_ (port)
	, usr_ (usr)
	, pwd_ (pwd)
	, mysql_ (NULL)
{
	if(db)
		db_ = string(db);
	//
	char buf[256];
	snprintf(buf, sizeof(buf), "%s:%u:%s", host, port, usr);
	connectionInfo_ = buf;
}

Connection::~Connection()
{
	close();
}

bool Connection::selectDB(const char * db)
{
	if(mysql_select_db(mysql_, db) != 0)
	{
		makeMysqlError();
		return false;
	}
	db_ = db;
	return true;
}

bool Connection::connect()
{
	mysql_ = mysql_init(NULL);
	if(NULL == mysql_)
	{
		makeError("Initialize mysql failed .(maybe insufficient memory)");
		return false;
	}
	// set auto reconnect
	my_bool reconnect = 1;
	if(mysql_options(mysql_, MYSQL_OPT_RECONNECT, &reconnect) != 0)
	{
		makeMysqlError();
		return false;
	}
	// set connect timeout
	unsigned int conn_timeout = 5;
	if(mysql_options(mysql_, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&conn_timeout) != 0)
	{
		makeMysqlError();
		return false;
	}
	// real connect
	const char *db = db_.empty() ? NULL : db_.c_str();
	if(mysql_real_connect(mysql_, host_.c_str(), usr_.c_str(), pwd_.c_str(), db, port_, NULL, 0) == NULL)
	{
		makeMysqlError();
		return false;
	}
	return true;
}

void Connection::close()
{
	if(mysql_)
	{
		mysql_close(mysql_);
		mysql_ = NULL;
	}
}

bool Connection::query(const char *sql)
{
	if(mysql_query(mysql_, sql) != 0)
	{
		string str;
		str += "(sql=";
		str += sql;
		str += ")";		
		makeMysqlError(str);
		return false;
	}
	return true;
}

bool Connection::del(const char *table, const char *condition)
{
	string sql("delete from ");
	sql.append(table);
	if(condition)
		sql.append(condition);
	return query(sql.c_str());
}
bool Connection::select(const char *table, const char *fields, const char *condition)
{
	string sql("select ");
	sql.append(fields);
	sql.append(" from ");
	sql.append(table);
	sql.append(" ");
	if(condition)
		sql.append(condition);
	return query(sql.c_str());
}

bool Connection::update(const char *table, const Fields &fields, const char *condition)
{
	int field_count = fields.count();
	if(field_count < 1)
		return true;
	// make SQL
	string sql("update ");
	sql.append(table);
	sql.append(" set ");
	for(int i = 0; i < field_count; ++i)
	{
		if(0 != i)
			sql.push_back(',');
		sql.append(fields[i].name());
		sql.append("=");
		if(fields[i].needQuote())
			sql.push_back('\'');		
		sql.append(fields[i].value());
		if(fields[i].needQuote())
			sql.push_back('\'');		
	}
	if(condition)
		sql.append(condition);
	return query(sql.c_str());
}

bool Connection::insert(const char *table, const Fields &fields)
{
	if(fields.count() <= 0)
	{
		makeError("empty fields");
		return false;
	}
	string sql;
	makeInsertSql(table, fields, sql);
	return query(sql.c_str());
}

bool Connection::insertOrUpdate(const char *table, const Fields &insert_fields, const Fields &update_fields)
{
	if(insert_fields.count() <= 0 || update_fields.count() <= 0)
	{
		makeError("empty insert fields or update field");
		return false;
	}
	// make SQL
	string sql;
	makeInsertSql(table, insert_fields, sql);
	//
	sql.append(" on duplicate key update ");
	for(int i = 0; i < update_fields.count(); ++i)
	{
		if(0 != i)
			sql.push_back(',');
		sql.append(update_fields[i].name());
		sql.append("=");
		if(update_fields[i].needQuote())
			sql.push_back('\'');		
		sql.append(update_fields[i].value());
		if(update_fields[i].needQuote())
			sql.push_back('\'');		
	}
	
	return query(sql.c_str());
}

void Connection::makeInsertSql(const char *table, const Fields &fields, std::string &sql)
{
	sql.clear();
	int field_count = fields.count();
	// make SQL
	sql = "insert into ";
	sql.append(table);
	// field 
	
	sql.push_back('(');
	for(int i = 0; i < field_count; ++i)
	{
		if(0 != i)
			sql.push_back(',');
		sql.append(fields[i].name());
	}
	sql.push_back(')');
	// values
	sql.append(" values(");
	for(int i = 0; i < field_count; ++i)
	{
		if(0 != i)
			sql.push_back(',');
		if(fields[i].needQuote())
			sql.push_back('\'');
		sql.append(fields[i].value());
		if(fields[i].needQuote())
			sql.push_back('\'');		
	}
	sql.push_back(')');	
}

ResultSetPtr Connection::getResultSet(bool store)
{
	ResultSetPtr rsp = ResultSet::create(mysql_, store);
	if(!rsp)
	{
		makeMysqlError();
	}
	return rsp;
}

long Connection::getAffectedRows()
{
	return static_cast<long>(mysql_affected_rows(mysql_));
}

bool Connection::setAutoCommit(bool isauto)
{
	my_bool mbauto = isauto ? 1 : 0;
	if(mysql_autocommit(mysql_, mbauto) != 0)
	{
		makeMysqlError();
		return false;
	}
	return true;
}

bool Connection::commit()
{
	if(mysql_commit(mysql_) != 0)
	{
		makeMysqlError();
		return false;
	}
	return true;
}

bool Connection::rollback()
{
	if(mysql_rollback(mysql_) != 0)
	{
		makeMysqlError();
		return false;
	}
	return true;
}

bool Connection::isTableExist(const char *table)
{	
	int ret;
	MYSQL_RES *res = mysql_list_tables(mysql_, table);	
	if(res == NULL)	
		ret = false;	
	if(mysql_num_rows(res) > 0)		
		ret = true;	
	else		
		ret = false;	
	mysql_free_result(res);	
	return ret;
}

void Connection::makeMysqlError(const std::string &errstr)
{
	errstr_.clear();
	
	errstr_ = mysql_error(mysql_);
	errstr_.append(", ");
	errstr_.append(errstr);
	errstr_.append(" on ");
	errstr_.append(connectionInfo_);
}

void Connection::makeError(const std::string &errstr)
{
	errstr_.clear();
	errstr_.append(errstr);
	errstr_.append(" on ");
	errstr_.append(connectionInfo_);
}

} /*end namespace CppMySql*/