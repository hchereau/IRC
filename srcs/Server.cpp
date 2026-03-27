#include "Server.hpp"
#include "ServerConfig.hpp"

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

void Server::confServer()
{
	ServerConfig config; // hardcoded one, i need to the initial parsing to get port etc

	_fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	// domain IPv4 주소 체계, sockets type TCP 이용, protocol 0은 type 에 지정한 값 이용
	if (_fdSocket == -1)
		sysError();
	struct sockaddr_in svAddr;
	std::memset(&svAddr, 0, sizeof(svAddr));
	svAddr.sin_family = AF_INET;
	svAddr.sin_addr.s_addr = INADDR_ANY;
	svAddr.sin_port = htons(8080);
	_binded = bind(_fdSocket, (struct sockaddr *)&svAddr, sizeof(svAddr)); // give the socket fd to local addr bind 용 캐스팅 필요
	if (_binded == -1)
		sysError();
	// listen // 연결 대기 상태 만들기, 연결 요청 대기열 실제 연결은 accept, backlog 는 동시에 대기열 쌓아둘 수 있는 연결 요청 개수 OS 가 max 강제 제한
	_listening = listen(_fdSocket, config.backlog);
	if (_listening == -1)
		sysError();
	// after socket bind listen server is open
	// * update server's fd to the poll list for the idx 0 - struct only for poll ? class with member function ?
}

void Server::recvServ()
{
	ServerConfig config;
	char* buff_hard[1024]; 

	recv(client->getFd(), buff_hard, sizeof(buff_hard), 0); // 데이터 받기
}

void Server::sendServ()
{
	ServerConfig config;
	char* buff_hard[1024]; 
	send(client->getFd(), buff_hard, sizeof(buff_hard), 0); // 데이터 보내기
}

void Server::runServer()
{
	struct sockaddr_in clientAddr; // 누가 접속할지 모르니 os 가 accept 할 때 채워줌
	std::memset(&clientAddr, 0, sizeof(clientAddr));
	ServerConfig config;
	// 클라 접속 처리 (accept)
	while (!g_sig) // make signal enums on somewhere ?
	{
		poll(fds, nfds, 1000);
		// 1) check the Server event from the first idx of poll list
		// 2) accept to get the new clients
		socklen_t client_addr_size = sizeof(clientAddr); // not hardcoded, how i get it?
		// getter inside of client ?
		// if (client_addr_size == -1) // maybe debug needed here ?
		// 	// ?
		int temp_clientFd = accept(_fdSocket, (struct sockaddr *)&clientAddr, &client_addr_size); // struct 에 저장할 것임
		// setter ?
		// 3) put the new clients on the poll list
		client->setAdd(temp_clientFd);
		if (temp_clientFd == -1)
			sysError();

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

int Server::disServer()
{
	// close()
}

void Server::sysError()
{
	// system call error (socket bind)
	// clean exit ? with which exit status ? 1 ?
	exit(1);
}