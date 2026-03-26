#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
// #include "Channel.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring> // memseset
#include <map>
#include <vector>
#include <set>
#include <sys/poll.h> // struct defined
#include <algorithm> // fill

typedef enum s_syserror
{
	ERR_SOCKET,
	ERR_BIND,
	ERR_LISTEN,
	ERR_ACCEPT,
	FAIL = -1
} t_syserror;

class Server {

	private:

	int	_port;
	std::string _password;

	int	_fdSocket;
	int	_binded; // hmm
	int _listening; // hmm

	std::map<int, Client> _clients; // _clients[fd] or iterator ..
	std::vector<struct pollfd> _polling;
	std::set<int> _todelFds;

	// getter
	void updPoll(void); // _pollFds
	void updClients(void);
	void recvServ(int fd);
	void sendServ(int fd);
	void broadCast(const std::string& msg, int notThisFd);
	void disClient(int fd); // remove _todelFds in _pollFds / _clients / if needed do close(fd)
	void sysError(int sys_enum);
	void cleanDown(); // when sig(global variable) catched while run server loop etc

	public:
	
	Server(int port, std::string password);
	Server(Server &other);
	Server& operator=(const Server &other);
	~Server();

	void confServer();
	std::map<int, Client>getClients(void) const;
	void runServer();

};

#endif