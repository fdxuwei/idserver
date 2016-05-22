#include <assert.h>
#include "ResultSet.h"

#define EMPTY_STRING ""

namespace cppmysql
{

ResultSet::ResultSet(MYSQL_RES *res)
	: res_ (res)
{
	
}

ResultSet::~ResultSet()
{
	if(res_)
	{
		mysql_free_result(res_);
	}
}


ResultSetPtr ResultSet::create(MYSQL *mysql, bool store)
{
	assert(mysql);
	MYSQL_RES *res;
	if(store)
	{
		res = mysql_use_result(mysql);
	}
	else
	{
		res = mysql_store_result(mysql);
	}
	if(res)
	{
		// 
		ResultSetPtr rsp(new ResultSet(res));
		rsp->setFieldIndex();
		return rsp;
	}
	else
	{
		// return null ptr
		return ResultSetPtr();
	}
}

bool ResultSet::moveNext()
{
	row_ = mysql_fetch_row(res_);
	return (NULL != row_);
}

int ResultSet::getCount()
{
	return mysql_num_rows(res_);
}


long ResultSet::getInteger(int index)
{
	if(NULL == row_[index])
		return 0;
	return str2Long(row_[index]);
}

unsigned long ResultSet::getUInteger(int index)
{
	if(NULL == row_[index])
		return 0;
	return str2ULong(row_[index]);
}


double ResultSet::getDouble(int index)
{
	if(NULL == row_[index])
		return 0.0;
	return str2Double(row_[index]);
}

const char * ResultSet::getString(int index)
{
	if(NULL == row_[index])
		return EMPTY_STRING;
	return row_[index];
}

long ResultSet::getInteger(const char *field)
{
	FieldIndexType::const_iterator it = field_index_.find(std::string(field));
	if(field_index_.end() == it)
		return 0;
	int index = it->second;
	if(NULL == row_[index])
		return 0;
	return str2Long(row_[index]);
}

unsigned long ResultSet::getUInteger(const char *field)
{
	FieldIndexType::const_iterator it = field_index_.find(std::string(field));
	if(field_index_.end() == it)
		return 0;
	int index = it->second;
	if(NULL == row_[index])
		return 0;
	return str2ULong(row_[index]);
}


double ResultSet::getDouble(const char *field)
{
	FieldIndexType::const_iterator it = field_index_.find(std::string(field));
	if(field_index_.end() == it)
		return 0.0;
	int index = it->second;
	if(NULL == row_[index])
		return 0.0;
	return str2Double(row_[index]);
}

const char * ResultSet::getString(const char *field)
{
	FieldIndexType::const_iterator it = field_index_.find(std::string(field));
	if(field_index_.end() == it)
		return EMPTY_STRING;
	int index = it->second;	
	if(NULL == row_[index])
		return EMPTY_STRING;
	return row_[index];
}


void ResultSet::setFieldIndex()
{
	MYSQL_FIELD *field;
	int index = 0;
	field_index_.clear();
	while((field = mysql_fetch_field(res_)))
	{
		field_index_[field->name] = index++;
	}
}

long ResultSet::str2Long(const char *str)
{
	return strtol(str, NULL, 10);
}

unsigned long ResultSet::str2ULong(const char *str)
{
	return strtoul(str, NULL, 10);
}


double ResultSet::str2Double(const char *str)
{
	return strtod(str, NULL);
}
	

} /* cppmysql */
