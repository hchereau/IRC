#include "Server.hpp"
#include "Client.hpp"

volatile std::sig_atomic_t sigFlag = 0;

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

Channel* Server::getChannelByName(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        return it->second;
    }
    return NULL;
}

void Server::addChannel(const std::string& name) {
    if (_channels.find(name) == _channels.end()) {
        _channels[name] = new Channel(name);
    }
}

void Server::removeChannel(const std::string& name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end())
	{
		delete it->second;
		_channels.erase(it);
	}
	// 채널 목록에서 없애구, 채널 delete dealloc 관리하기
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

int Server::isDis(int fd)
{
	// based on the fd check pollfd's revents flag
	std::vector<struct pollfd>::iterator it = _polling.begin() + 1;
	while (it != _polling.end())
	{
		if (fd == it->fd)
		{
			if (it->revents & POLLHUP || it->revents & POLLERR || it->revents & POLLNVAL)
			return (1);
		}
		++it;
	}
	return (0);
}

void Server::recvServ(int fd, int *i)
{
	//_readBuffer (읽다 만 데이터 보관)을 recv 로 읽기
	char tempBuff[1024];
	while (1)
	{
		int ret = recv(fd, tempBuff, sizeof(tempBuff), 0);
		if (ret < 0) // ret < 0 && errno ==EGAIN/EWOULDBLOCK 이래야 지금 읽을 거 끝이라 하네
		{
			if (errno == EINTR)
				continue ;
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break ;
			_todelFds.insert(_polling[*i].fd);
			break ;
		}
		else if (ret == 0) // 상대 종료
		{
			_todelFds.insert(_polling[*i].fd);
			break ;
		}
		std::map<int, Client*>::iterator it = _clients.find(fd);
		if (it != _clients.end())
		{
			Client* client = it->second;
			client->appendToReadBuffer(std::string(tempBuff, ret));
			if (client->getReadBuffer().size() > MAX_READ_BUFF) // message too long ? without finding \r\n i donno if its needed
			{
				_todelFds.insert(_polling[*i].fd);
				break ;
			}
			size_t pos; // 없으면 std::string::nps
			while ((pos = client->getReadBuffer().find("\r\n")) != std::string::npos)
			{
				if (pos > MAX_ONE_MESSAGE) // > need to check the condition (> 512)
				{
					_todelFds.insert(_polling[*i].fd);
					break ;
				}
				std::string oneLine = client->extractMessage();
				if (isDis(fd))	
				{
					_todelFds.insert(_polling[*i].fd);
					break ;
				}
				if (!(oneLine == ""))
					toParser(fd, oneLine);
			}
		}
	}
}

void Server::sendServ(int fd, int *i)
{
	// _writeBuffer(쓰다 만 데이터 보관) 를 send 로 비워야 함
	Client* client = _clients[fd]; // should be sure if this fd on the map
	std::string& wbuff = client->getWriteBuffer();
	while (!wbuff.empty())
	{
		int ret = send(fd, wbuff.c_str(), wbuff.size(), 0);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue ;
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break ;
			_todelFds.insert(_polling[*i].fd);
			break ;
		}
		if (ret == wbuff.size())
			setPolling(fd, set_POLLIN);
		wbuff.erase(0, ret);
	}
}

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

Client* Server::getClientByFd(int fd)
{
	// return _clients.at(fd); ++11 lol
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return it->second;
	return NULL;
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

void Server::sysError(int sys_enum)
{
	std::string errors[ERR_count] = {"socket", "bind", "listen", "accept", "poll"};
	std::cerr << "Error_systemcall: " << errors[sys_enum] << " " << strerror(errno) << std::endl;
	if (sys_enum == ERR_ACCEPT)
		return ;
	exit(1);
}

void Server::cleanDown() // 더 지울거 생각해보깅
{
	std::map<std::string, Channel*>::iterator it = _channels.begin();
	for (_channels.begin(); it != _channels.end(); ++it)
	{
		delete(it->second);
	}
	std::map<int, Client*>::iterator c_it = _clients.begin();
	for (_clients.begin(); c_it != _clients.end(); ++c_it)
	{
		delete(c_it->second);
		close(c_it->first);
	}
	close (_fdSocket);
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

void sigHandler(int sig)
{
    sigFlag = 1;
}

void sigSet(void)
{
    std::signal(SIGINT, sigHandler);
    std::signal(SIGTERM, sigHandler);
    std::signal(SIGPIPE, SIG_IGN);
}

void Server::runServer()
{
	while (!sigHandler)
	{
		int fdPerform = poll(_polling.data(), _polling.size(), 1000);
		if (fdPerform < 0)
			sysError(ERR_POLL);
		if (fdPerform == 0)
		{	
			timeOut();
			continue ;
		}
		if (_polling[0].revents & POLLIN) // revents is bitmask so &
			updPoll();
		int i = 1;
		std::vector<struct pollfd>::iterator it = _polling.begin() + 1;
		while (it != _polling.end())
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
		if (_polling[i].fd == fd && flag == set_POLLIN)
			_polling[i].events = POLLIN;
		else if (_polling[i].fd == fd && flag == set_POLLOUT)
			_polling[i].events = POLLOUT;
		else if (_polling[i].fd == fd && flag == set_POLLHUP)
			_polling[i].events = POLLHUP;
		++it;
		++i;
	}
}
