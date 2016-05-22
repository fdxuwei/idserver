#ifndef ID_SERVER_H
#define ID_SERVER_H

#include <stdint.h>
#include <string>

/**********************************************************
* example code:
*	//
*	using namespace idserver
*	uint64_t id;
*	if(!getId(id))
*	{
*		LOG << errStr();
*		return;
*	}
*	//
*	...
*
***********************************************************/

namespace idserver
{
	/**
	 * \brief init
	 *
	 * This function initialize address and business type of id server.
	 *
	 * \param ip ipv4 address.
	 *
	 * \param port the port of id server.
	 *
	 * \param bizType business type.One id server support more applications, bizType is used to distinguish them
	 *
	 * \thread safety: not safe, you should call it before any other API of idserver.
	 */	
	void init(const std::string &ip, int port, int bizType);

	/**
	 * \brief getId
	 *
	 * This function get an unique id from server.
	 *
	 * \param id the return id.
	 *
	 * \return if return false, call errStr() to get the error message.
	 *
	 * \thread safety: safe
	 */	
	bool getId(uint64_t &id);

	/**
	 * \brief setCacheNum
	 *
	 * This function set the id number to cache, the default is 100.
	 *
	 * \param num the cache number.
	 *
	 * \thread safety: safe
	 */
	void setCacheNum(int num);

	/**
	 * \brief errStr
	 *
	 * This function return the error message of last failed call.
	 *
	 * \return the error message.
	 *
	 * \thread safety: safe
	 */	
	const std::string &errStr();
};

#endif