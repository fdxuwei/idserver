#ifndef _MODULE_DAO_SERVER_H_
#define _MODULE_DAO_SERVER_H_

namespace dao
{
namespace mysql
{

class Server
{
public:
	//
	Server(const std::string &name, const std::string &host, unsigned short port, const std::string &usr, const std::string &pwd)
		: name_ (name)
		, host_ (host)
		, port_ (port)
		, usr_ (usr)
		, pwd_ (pwd)
		{}
	/* get properties */
	const std::string & name() const { return name_; }
	const std::string & host() const { return host_; }
	unsigned short port() const { return port_; }
	const std::string & usr() const { return usr_; }
	const std::string & pwd() const { return pwd_; }
private:
	std::string name_;
	std::string host_;
	unsigned short port_;
	std::string usr_;
	std::string pwd_;
};

}
}
#endif