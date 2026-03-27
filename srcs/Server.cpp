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
	if (temp_clientFd == FAIL)
	{	
		sysError(ERR_ACCEPT);
		return ;
	}
	fcntl(temp_clientFd, F_SETFL, O_NONBLOCK);
	pollfd add_to_polling;
	add_to_polling.fd = temp_clientFd;
	add_to_polling.events = POLLIN;
	add_to_polling.revents = 0;
	std::string clientHostname(inet_ntoa(clientAddr.sin_addr));
	Client* client = new Client(temp_clientFd, clientHostname); // leak management needed
	_clients.insert(std::make_pair(temp_clientFd, client));
	_polling.push_back(add_to_polling);
}

std::map<int, Client*> Server::getClients(void) const
{
	return this->_clients;
}


const std::map<int, Client*>& Server::congetC(void) const
{
	return this->_clients;
}

void Server::recvServ(int fd)
{
	char* tempBuff[1024]; 
	// recv(검색하기, tempBuff, sizeof(tempBuff), 0); // 데이터 받기
}

void Server::sendServ(int fd)
{
	char* tempBuff[1024]; 
	// send(검색하기, tempBuff, sizeof(tempBuff), 0); // 데이터 보내기

}

void Server::broadCast(const std::string& msg, int notThisFd)
{

}

void Server::delClients(void)
{
	std::vector<struct pollfd>::iterator p_it = _polling.begin() + 1;
	while (p_it != _polling.end())
	{
		if (_todelFds.find(p_it->fd) != _todelFds.end()) // is this fd inside of _todelFds
			p_it = _polling.erase(p_it);
		else
			++p_it;
	}
	std::set<int>::iterator c_it = _todelFds.begin();
	while (c_it != _todelFds.end())
	{
		int fd = *c_it;
		if (_clients.find(fd) != _clients.end())
		{
			delete(_clients[fd]);
			_clients.erase(fd);
			close(fd);
		}
		++c_it;
	}
	_todelFds.clear();
}

	// close()

void Server::sysError(int sys_enum)
{
	std::string errors[ERR_count] = {"socket", "bind", "listen", "accept", "poll"};
	std::cerr << "Error_systemcall: " << errors[sys_enum] << strerror(errno) << std::endl;
	if (ERR_ACCEPT)
		return ;
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
		int i = 1;
		std::vector<struct pollfd>::iterator it = _polling.begin() + 1;
		while (it != _polling.end())
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
		}
		++it;
		++i;
		// look at the "delete" list and delete
		delClients();
	}
	// signal occured
	cleanDown();
}

void Server::setPolling(int fd, int flag)
{
	int i = 1;
	std::vector<struct pollfd>::iterator it = _polling.begin() + 1;
	while (it != _polling.end())
	{
		if (_polling[i].fd = fd && flag == set_POLLOUT)
			_polling[i].events = POLLOUT;
		else if (_polling[i].fd = fd && flag == set_POLLHUP)
			_polling[i].events = POLLOUT;
		return ;
		++it;
		++i;
	}
}
