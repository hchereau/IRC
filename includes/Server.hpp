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
#include <cerrno> // syscall error part
#include <cstring> // syscall error part
#include <arpa/inet.h> // inet_ntoa
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h> // debug printf
#include <csignal>

typedef enum s_syserror
{
	ERR_SOCKET,
	ERR_BIND,
	ERR_LISTEN,
	ERR_ACCEPT,
	ERR_POLL,
	ERR_count,
	FAIL = -1
} t_syserror;

typedef enum s_eventflag
{
	set_POLLOUT,
	set_POLLHUP
} t_eventflag;

class Server {

	private:

	int	_port;
	std::string _password;

	int	_fdSocket;
	int	_binded; // hmm
	int _listening; // hmm

	std::map<int, Client*> _clients; // _clients[fd] or iterator ..
	std::vector<struct pollfd> _polling;
	std::set<int> _todelFds;

	void timeOut(void);
	void updPoll(void); // _pollFds and _clients
	void recvServ(int fd, int *i);
	void sendServ(int fd, int *i);
	void broadCast(const std::string& msg, int notThisFd);
	void delClients(void); // remove _todelFds in _pollFds / _clients / if needed do close(fd)
	void sysError(int sys_enum);
	void cleanDown(); // when sig(global variable) catched while run server loop etc

	public:
	
	Server(int port, std::string password);
	Server(Server &other);
	Server& operator=(const Server &other);
	~Server();

	void confServer(void);
	std::map<int, Client*> getClients(void) const;
	const std::map<int, Client*>& congetC(void) const;

	void runServer(void);
	void setPolling(int fd, int flag);

};

#endif