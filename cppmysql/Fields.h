#ifndef CPP_MYSQL_FIELDS_H
#define CPP_MYSQL_FIELDS_H

#include <map>
#include <vector>
#include <string>
#include <sstream>

namespace cppmysql
{
// single field
class Field
{
	friend class Fields;
public:
	const char* name() const {return name_.c_str(); }
	const char* value() const {return value_.c_str(); }
	bool needQuote() const {return needQuote_; }
protected:
	Field(const char *name, const char *value) // only  Fields class can create
		: name_ (name)
		, value_ (value)
		, needQuote_ (true)
		{}
	std::string name_;
	std::string value_;
	bool needQuote_;
};
// Fields class , for update or insert
class Fields
{
public:
	~Fields();
	template<class VT>
	void set(const char *name, VT value);
	void set(const char *name, const char *value);
	void set(const char *name, const std::string &value, bool needQuote);
	int count() const {return fields_.size(); }
	const Field& operator[] (int index) const;
private:
	std::vector<Field*> fields_;
	// field name index
	std::map<std::string, int> field_index_;
};


// specialize for std::string
template<>
inline void Fields::set(const char *name, const std::string &value)
{
	set(name, value.c_str());
}


// template functions
template<class VT>
void Fields::set(const char *name, VT value)
{
	// translate value to string
	std::stringstream ss;
	ss << value;
	set(name, ss.str().c_str());
}


	
}

#endif
