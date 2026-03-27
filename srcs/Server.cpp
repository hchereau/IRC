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

// Server& Server::operator=(const Server &other)
// {

// }

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

void Server::recvServ(int fd, int *i)
{
	//_readBuffer (읽다 만 데이터 보관)을 recv 로 읽기
	char tempBuff[1024];
	while (읽을게 남아 있는 동안..)
	{
		int ret = recv(fd, tempBuff, sizeof(tempBuff), 0);
		if (ret < 0) // ret < 0 && errno ==EGAIN/EWOULDBLOCK 이래야 지금 읽을 거 끝이라 하네
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
			{
				// set timeout and continue
			}
			else if (errno == -1) // xxxx wtf is this if < 0 and if == -1 lol 존나 이상함;;;;
				_todelFds.insert(_polling[*i].fd);
		}
		else if (ret == 0) // 상대 종료
		{
			_todelFds.insert(_polling[*i].fd);
			// FIN or RTS from the client so socket close
		}
		else // (ret > 0) 이면 계속 append
		{
			여기에 해당하는 client 는 fd 를 map 에다 찾아낸 애
			client->appendToReadBuffer(const std::string& data);
			한번에 모든 메시지를 다 받았다고 장담하지 말것
			버퍼 사이즈만큼 꽉 차게 받았다면 다시 받아본다든지.. 
			한번 recv에 하나의 메시지 패킷만 수신되지 않을 수 있음 아마 이건 \r\n 으로 끊으면 될듯
			수신 버퍼 크기가 100인데 60짜리 메시지 패킷이 연달아 2개 와서 60+40으로 리턴 등
		}
		//this function return the size of the message on successful completion
		// if no messages are available at the socket, the receive calls wait for a m
	}
	// 버퍼가 비었든 안 비었든 대게 POLLIN 은 안끔 읽기 이벤트는 항상 감시~
	// recv 쪽도 처리된 부분은 버퍼에서 제거하지만 버퍼 비었다고 플래그 끄는 건 아님 ㅋㅋ
}

void Server::sendServ(int fd, int *i)
{
	// _writeBuffer(쓰다 만 데이터 보관) 를 send 로 비워야 함
	char tempBuff[1024]; 
	while (42)
	{
		int sent = send(fd, tempBuff, sizeof(tempBuff), 0);
		if (sent < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
			{
				// set timeout and continue
			}
			else if (errno == -1)
				_todelFds.insert(_polling[*i].fd);
		}
		else if (errno == -1)
			_todelFds.insert(_polling[*i].fd);
		else if (sent == 0)
		{	
			// SIGPIPE 처리 관련, SIGPIPE signal 을 받고 프로세스가 죽어버리는 경우
			// signal(SIGPIPE, SIG_IGN); 을 프로그램 시작 직후에 넣어두면 
			// send 실행 시 연결이 끊어져 있어도 프로세스를 종료하지 않고 단순히 -1을 반환함 -> 그렇게 해둠
			_todelFds.insert(_polling[*i].fd);
		}
		else
		{
			void	appendToWriteBuffer(const std::string& data);
			보내는 패킷의 길이만큼 모두다 보내는데 성공했는가
			부족한 경우 
			{
				while (timeout)	
					timeout 만큼 재전송 로직 구현
				혹은 폐기 등..
			}
		}
	}
	// 다 보내졌으면 비우고 POLLOUT 끔
} // 그 외에 EINTR 은 인터럽트가 발생하여 send 나 recv 가 빠져나오는 경우는 뭔진 모르겠지만 처리할
// 일인지 체크하기

// execution logics and uses, on the server class
void Server::privateMsg(const std::string& msg)
{
	닉네임으로 target client 찾아서
	그 사람 writeBuffer 에 append 
	POLLOUT 켜기	
}

// this one maybe no need
void Server::channelMsg(const std::string& msg)
{
	Channel* channel = getterChannel ?; 
	channel->broadcastMessage(msg, sender);
	채널 멤버들 순회
	sender 제외
	각자의 writebuffer 에 append 
	POLLOUT 켜기
}

// server broadCast
void Server::broadCastAll(const std::string& msg, int notThisFd)
{
	_clients 순회해서
	보낸 사람 제외하고
	각 대상의 appendToWriteBbuffer(msg)
	각 대상 fd 에 POLLOUT 켜기
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
	std::cerr << "Error_systemcall: " << errors[sys_enum] << " " << strerror(errno) << std::endl;
	if (sys_enum == ERR_ACCEPT)
		return ;
	exit(1);
}

void Server::cleanDown()
{

}

void Server::confServer()
{
	std::signal(SIGPIPE, SIG_IGN);
	_fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_fdSocket == FAIL)
		sysError(ERR_SOCKET);

	struct sockaddr_in svAddr;
	std::memset(&svAddr, 0, sizeof(svAddr));
	svAddr.sin_family = AF_INET;
	svAddr.sin_addr.s_addr = INADDR_ANY;
	svAddr.sin_port = htons(_port); // 생성자에서 port 받을텐데

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
	printf("%i\n", _polling[0].fd);
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
		while (it != _polling.end()) // 얘네 지금 poll 제대로 안되고 있음!
		{
			if (_polling[i].revents & POLLIN)
			{
				recvServ(_polling[i].fd , &i);
			}
			else if (_polling[i].revents & POLLOUT)
			{
				sendServ(_polling[i].fd, &i);
			}
			else if (_polling[i].revents & POLLHUP)
			{
				_todelFds.insert(_polling[i].fd);
			}
			++it;
			++i;
		}
		delClients();
	}
	// signal occured or quit kind of ?
	cleanDown();
}

void Server::setPolling(int fd, int flag)
{
	// caller to put flag or i can divide with more evident name for each case
	int i = 1;
	std::vector<struct pollfd>::iterator it = _polling.begin() + 1;
	while (it != _polling.end())
	{
		if (_polling[i].fd == fd && flag == set_POLLOUT)
			_polling[i].events = POLLOUT;
		else if (_polling[i].fd == fd && flag == set_POLLHUP)
			_polling[i].events = POLLHUP;
		++it;
		++i;
	}
}
