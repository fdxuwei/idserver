#ifndef CPPMYSQL_RESULTSET_H
#define CPPMYSQL_RESULTSET_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include <mysql/mysql.h>

namespace cppmysql
{

class ResultSet;
typedef boost::shared_ptr<ResultSet> ResultSetPtr;
// resultset for mysql
class ResultSet
{
	friend class Connection;
public:
	~ResultSet();	
	bool moveNext();
	// get field values,  by field name or field index , field index start from 0.
	// get by index is faster than get by name
	long getInteger(const char *field);
	long getInteger(int index);
	unsigned long getUInteger(const char *field);
	unsigned long getUInteger(int index);	
	double getDouble(const char *field);
	double getDouble(int index);
	const char* getString(const char *field);
	const char* getString(int index);	
	int getCount();
	
protected:	
	static ResultSetPtr create(MYSQL *mysql, bool store);	
private:	
	ResultSet(MYSQL_RES *res);
	// set the map of field name to field index
	void setFieldIndex();
	long str2Long(const char * str);
	unsigned long str2ULong(const char * str);
	double str2Double(const char * str);
	MYSQL_RES *res_;
	MYSQL_ROW row_;
	// 
	typedef std::map<std::string, int> FieldIndexType;
	FieldIndexType field_index_;
};


}/*end namespace cppmysql*/

#endif

