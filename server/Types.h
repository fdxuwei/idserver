#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t IdType;

struct CachedIds
{
	CachedIds()
		: nextId_ (0)
		, maxId_ (0){}
	IdType nextId_;
	IdType maxId_;
};

#endif