#ifndef BLOCK_TCP_CLIENT_H
#define BLOCK_TCP_CLIENT_H

#include <string>

class BlockTcpClient
{
public:
	BlockTcpClient(const std::string &ip, int port);
	~BlockTcpClient();
	void setTimeout(int timeout);
	bool connect();
	bool send(const char *data, int size);
	bool recv(char *data, int size);
	void close() {closeFd(); }
	const std::string &errstr() const {return errstr_; }
private:
	//
	void closeFd();
	//
	std::string ip_;
	int port_;
	int timeout_;
	std::string errstr_;
	int fd_;
};


#endif