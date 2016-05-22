#include <assert.h>
#include "Fields.h"

using namespace std;

namespace cppmysql
{
Fields::~Fields()
{
	for(int i = 0; i < fields_.size(); ++i)
		delete fields_[i];
	fields_.clear();
	field_index_.clear();
}

void Fields::set(const char *name, const char *value)
{
	// find existing name
	std::string namestr = name;
	std::map<std::string, int>::iterator it = field_index_.find(namestr);
	if(field_index_.end() == it)
	{
		// not exist, create it
		Field *fp = new Field(name, value);
		fp->needQuote_ = true;
		fields_.push_back(fp);
		field_index_[namestr] = fields_.size()-1;
	}
	else
	{
		// already exist, update it
		fields_[it->second]->value_ = value;
		fields_[it->second]->needQuote_ = true;
	}

}

void Fields::set(const char *name, const std::string &value, bool needQuote)
{
	// find existing name
	std::string namestr = name;
	std::map<std::string, int>::iterator it = field_index_.find(namestr);
	if(field_index_.end() == it)
	{
		// not exist, create it
		Field *fp = new Field(name, value.c_str());
		fp->needQuote_ = needQuote;
		fields_.push_back(fp);
		field_index_[namestr] = fields_.size()-1;
	}
	else
	{
		// already exist, update it
		fields_[it->second]->value_ = value;
		fields_[it->second]->needQuote_ = needQuote;
	}

}


const Field& Fields::operator[](int index) const
{
	assert(index < fields_.size());
	return (*fields_[index]);
}



}


