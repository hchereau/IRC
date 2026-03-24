#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring> // memseset

// poll struct or inside of hpp

class Server{

	private:

	int	_port;
	int	_fdSocket;
	int	_binded;
	int _listening;
	int	_clientFd;
	
	int _accepted; // accept 반환값은 client 랑 소통하기 위한 fd 인데, 

	Client *client; // 여러개가 처리되도록 해야함.. 얘 하나는 하나의 정보값으로..

	public:
	Server();
	Server(int port);
	Server(Server &other);
	Server& operator=(const Server &other);
	~Server();

	void confServer();
	void recvServ();
	void sendServ();
	void runServer();
	int disServer(); // disconnect int or void ?
	void sysError();
};

#endif