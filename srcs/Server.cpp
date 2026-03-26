#include "Server.hpp"
#include "Client.hpp"

// Server::Server() : _port(-1), _fdSocket(-1), _binded(-1), _listening(-1), _accepted(-1)
// {
// 	// others are set to fail flag, port is from 0 so just -1
// }

Server::Server(int port, std::string password) : _port(port), _password(password), _fdSocket(-1), _binded(-1), _listening(-1)
{

}

Server::Server(Server &other)
{
	if (this != &other)
	{
		this->_port = other._port;
		this->_fdSocket = other._fdSocket;
		this->_binded = other._binded;
		this->_listening = other._listening;
	}
}

Server& Server::operator=(const Server &other)
{

}

Server::~Server()
{

}

void Server::timeOut(void)
{
	// i donno how to deal with timeout
}

void Server::updPoll(void)
{
	struct sockaddr_in clientAddr; // 누가 접속할지 모르니 os 가 accept 할 때 채워줌
	std::memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t client_addr_size = sizeof(clientAddr);

	int temp_clientFd = accept(_fdSocket, (struct sockaddr *)&clientAddr, &client_addr_size);
	pollfd add_to_polling;
	if (temp_clientFd == FAIL)
		sysError(ERR_ACCEPT);
	add_to_polling.fd = temp_clientFd;
	add_to_polling.events = POLLIN;
	add_to_polling.revents = 0;
	Client* client = new Client(temp_clientFd); // leak management needed
	_clients[temp_clientFd] = client;
	_polling.push_back(add_to_polling);
}

void Server::updClients(void)
{

}

std::map<int, Client> Server::getClients(void) const
{
	return this->_clients;
}

void Server::recvServ(int fd)
{
	char* buff_hard[1024]; 
	// recv(client->getFd(), buff_hard, sizeof(buff_hard), 0); // 데이터 받기
}

void Server::sendServ(int fd)
{
	char* buff_hard[1024]; 
	// send(client->getFd(), buff_hard, sizeof(buff_hard), 0); // 데이터 보내기
}

void Server::broadCast(const std::string& msg, int notThisFd)
{

}

void Server::disClient(int fd)
{
	// close()
}

void Server::sysError(int sys_enum)
{
	std::string errors[ERR_count] = {"socket", "bind", "listen", "accept", "poll"};
	std::cerr << "Error_systemcall: " << errors[sys_enum] << strerror(errno) << std::endl;
	exit(1);
}

void Server::cleanDown()
{

}

void Server::confServer()
{
	_fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_fdSocket == FAIL)
		sysError(ERR_SOCKET);

	struct sockaddr_in svAddr;
	std::memset(&svAddr, 0, sizeof(svAddr));
	svAddr.sin_family = AF_INET;
	svAddr.sin_addr.s_addr = INADDR_ANY;
	svAddr.sin_port = htons(8080);

	_binded = bind(_fdSocket, (struct sockaddr *)&svAddr, sizeof(svAddr)); // give the socket fd to local addr, cast for bind
	if (_binded == FAIL)
		sysError(ERR_BIND);

	_listening = listen(_fdSocket, SOMAXCONN); // client can connect() from now on
	if (_listening == FAIL)
		sysError(ERR_LISTEN);

	// * update server's fd to the poll list for the idx 0
	struct pollfd temp_pollfd;
	temp_pollfd.fd = _fdSocket;
	temp_pollfd.events = POLLIN;
	temp_pollfd.revents = 0;
	
	std::vector<struct pollfd>::iterator it = _polling.begin();
	_polling.insert(it, temp_pollfd);
}

void Server::runServer()
{
	// while (!g_sig) // define signal (enums, global variable somewhere)
	while (1)
	{
		int serverEvent = poll(_polling.data(), 1, 1000);
		if (serverEvent < 0)
			sysError(ERR_POLL);
		if (serverEvent == 0)
			timeOut();
		if  (serverEvent == 1)
		{
			if (_polling[0].revents & POLLIN) // revents is bitmask so &
				updPoll();
		}
		int i = 0;
		while (std::vector<int>::iterator it = _polling.begin() + 1; it != _polling.end(); ++it)
		{
			// read and do the action as needed use enum or signal
			// 1) recv
			if (_polling[i].revents & POLLIN)
			{

			}
			// 2) send
			else if (_polling[i].revents & POLLOUT)
			{

			}
			// 3) disconnected client : update the "delete" list
			else if (_polling[i].revents & POLLHUP)
			{
				_todelFds.insert(_polling[i].fd);
			}
			// (getting EXECUTION result based(it should contain Client info) on the Client and ***treat it***)
		}	i++;
		// look at the "delete" list and delete
	}
	// signal occured
	cleanDown();
}

void Server::setPolling(int fd, int flag)
{
	// based on one fd, set to POLLOUT or disconnect 
	// caller to put each fd of the client if needed
	// flag is for the POLLOUT or disconnect setup 
}
