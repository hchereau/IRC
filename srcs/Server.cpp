#include "Server.hpp"

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

void Server::updPoll(void)
{
	
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
	if (sys_enum == ERR_SOCKET)
	{

	}
	else if (sys_enum == ERR_BIND)
	{

	}
	else if (sys_enum == ERR_LISTEN)
	{

	}
	else if (sys_enum == ERR_ACCEPT)
	{

	}
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
	struct sockaddr_in clientAddr; // 누가 접속할지 모르니 os 가 accept 할 때 채워줌
	std::memset(&clientAddr, 0, sizeof(clientAddr));

	// while (!g_sig) // define signal (enums, global variable somewhere)
	while (1)
	{
		int serverEvent = poll(_polling.fds, 1, 1000);
		if  (serverEvent == 1)
		{
			if (_polling.revents && POLLIN)
			{

			}

		}
		// 1) check the Server event from the first idx of poll list
	
		// 2) accept to get the new clients
		socklen_t client_addr_size = sizeof(clientAddr);
		// accept 는 server fd 에 이벤트가 있을 때만 하기
		int temp_clientFd = accept(_fdSocket, (struct sockaddr *)&clientAddr, &client_addr_size);
		// 3) put the new clients on the poll list
		client->setAdd(temp_clientFd);
		if (temp_clientFd == FAIL)
			sysError(ERR_ACCEPT);

		while (poll list till the end)
		{
			// read and do the action as needed use enum or signal
			// 1) recv
			// 2) send
			// 3) disconnected client : update the "delete" list
			// (getting EXECUTION result based(it should contain Client info) on the Client and ***treat it***)
		}
		// look at the "delete" list and delete
	}
}
