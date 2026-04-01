#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Channel.hpp"
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
#include <atomic>

#define MAX_READ_BUFF 9999 // need to think of the exact value
#define MAX_ONE_MESSAGE 512

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
	set_POLLIN,
	set_POLLOUT,
	set_POLLHUP
} t_eventflag;

class Server {

	private:

	int	_port;
	std::string _password;

	int	_fdSocket;

	std::map<int, Client*> _clients;
	std::vector<struct pollfd> _polling;
	std::set<int> _todelFds;
	std::map<std::string, Channel*> _channels;

	void updPoll(void); // _pollFds and _clients
	void recvServ(int fd, int *i);
	void sendServ(int fd, int *i);
	void privateMsg(const std::string& targetNick, const std::string& msg);
	void channelMsg(const std::string& name, Client* sender, const std::string& msg);
	void broadCastAll(const std::string& msg, int notThisFd);
	void delInChannel(void);
	void delClients(void); // remove _todelFds in _pollFds / _clients / if needed do close(fd)
	void sysError(int sys_enum);
	void cleanDown(); // when sig(global variable) catched while run server loop etc

	public:
	
	Server(int port, std::string password);
	~Server();


	void confServer(void);
	void runServer(void);

	void setPolling(int fd, int flag);
	int  isDis(int fd);

	std::map<int, Client*> getClients(void) const;
	const std::map<int, Client*>& congetC(void) const;

	// Clients management
	const std::string& getPassword(void) const;
	Client* getClientByFd(int fd);
	Client* getClientByNick(const std::string& name);


	// Channels management
	void    getOrCreateChannel(const std::string& name);
	void    addChannel(const std::string& name);
	void    removeChannel(const std::string& name);
	Channel* getChannelByName(const std::string& name);

};

#endif