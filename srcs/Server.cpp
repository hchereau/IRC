
#include "Server.hpp"
#include "Client.hpp"

volatile std::sig_atomic_t sigFlag = 0;

Server::Server(int port, std::string password) : _port(port), _password(password), _fdSocket(-1)
{

}

Server::~Server()
{

}

Channel* Server::getOrCreateChannel(const std::string& name)
{
	Channel* channel = getChannelByName(name);
	if (channel != NULL)
		return (channel);
	addChannel(name);
		return (getChannelByName(name));
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

void Server::acceptNewClient(void)
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
	if (fcntl(temp_clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(temp_clientFd);
		return ;
	}
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
					toParser(fd, oneLine); // a verifier
			}
		}
	}
}

void Server::sendServ(int fd, int *i)
{
	// _writeBuffer(쓰다 만 데이터 보관) 를 send 로 비워야 함
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return ;
	Client* client = it->second;
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
void Server::privateMsg(const std::string& targNick, const std::string& msg)
{
	Client* targ = getClientByNick(targNick);
	if (!targ)
		return ; // need to check the protocol
	targ->appendToWriteBuffer(msg);
	setPolling(targ->getFd(), set_POLLOUT);
}

void Server::channelMsg(const std::string& name, Client* sender, const std::string& msg)
{
	Channel* thisChannel = getChannelByName(name);
	if (!thisChannel)
		return ;
	thisChannel->broadcastMessage(msg, sender);

	const std::vector<Client*>& members = thisChannel->getMembers();
	for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (*it != sender)
			setPolling((*it)->getFd(), set_POLLOUT);
	}
}

// server broadCast
void Server::broadCastAll(const std::string& msg, int notThisFd)
{
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first == notThisFd)
			continue;
		it->second->appendToWriteBuffer(msg);
		setPolling(it->second->getFd(), set_POLLOUT);
	}
}

const std::string& Server::getPassword(void) const// go const or better this way?
{
	return this->_password;
}

Client* Server::getClientByFd(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end())
		return it->second;
	return NULL;
}

Client* Server::getClientByNick(const std::string& nickname)
{
	std::map<int, Client*>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

// to debug, dont forget to earase 

// void Server::debug_delInChannel()
// {
// 	std::map<std::string, Channel*>::iterator ch_it = _channels.begin();
// 	ch_it = _channels.begin();
// 	int i = 0;
// 	while (ch_it != _channels.end())
// 	{
// 		std::cout << "channel name: " << ch_it->first << std::endl;

// 		std::vector<Client*> clients = ch_it->second->getMembers();
// 		std::vector<Client*>::iterator it = clients.begin();
// 		while (it != clients.end())
// 		{
// 			const int fd = (*it)->getFd();
// 			std::cout << "Channel has these fds: " << fd << std::endl;
// 			++it;
// 		}
// 		++ch_it;
// 		++i;
// 	}
// 	std::cout << "theres " << i << " numbers of channels" << std::endl;
// }

void Server::delInPolling(void)
{
	std::vector<struct pollfd>::iterator it = _polling.begin() + 1;
	while (it != _polling.end())
	{
		if (_todelFds.find(it->fd) != _todelFds.end()) // is this fd inside of _todelFds
			it = _polling.erase(it);
		else
			++it;
	}
}

void Server::delInChannel(void)
{
	// debug_delInChannel();
	// first find and remove the matching clinet(s) using todelFds, on each channels  
	std::map<std::string, Channel*>::iterator ch_it = _channels.begin();
	while (ch_it != _channels.end())
	{
		Channel* channel = ch_it->second;
		std::set<int>::iterator it = _todelFds.begin();
		while (it != _todelFds.end())
		{
			int fd = *it;
			if (_clients.find(fd) != _clients.end())
			{
				channel->removeClient(_clients[fd]);
			}
			++it;
		}
		it = _todelFds.begin();
		++ch_it;
	}
	// then verify if theres no empty channel
	ch_it = _channels.begin();
	while (ch_it != _channels.end())
	{
		Channel* emp_channel = ch_it->second;
		if (emp_channel->isEmpty()) // this channel is empty after deleting the clients on _todelFds
		{
			delete emp_channel;
			_channels.erase(ch_it++);
		}
		else
		++ch_it;
	}
	// debug_delInChannel();
}

void Server::delClients(void)
{
	std::set<int>::iterator it = _todelFds.begin();
	while (it != _todelFds.end())
	{
		int fd = *it;
		if (_clients.find(fd) != _clients.end())
		{
			delete(_clients[fd]);
			_clients.erase(fd);
			close(fd);
		}
		++it;
	}
}

void Server::disconnectClients(void)
{
	delInPolling();
	delInChannel();
	delInClients();
	_todelFds.clear();
}

void Server::sysError(int sys_enum)
{
	std::string errors[ERR_count] = {"socket", "bind", "listen", "accept", "poll"};
	std::cerr << "Error_systemcall: " << errors[sys_enum] << " " << strerror(errno) << std::endl;
	if (sys_enum == ERR_ACCEPT)
		return ;
	cleanDown();
	exit(1);
}

void Server::cleanDown()
{
	std::map<std::string, Channel*>::iterator it = _channels.begin();
	for (; it != _channels.end(); ++it)
	{
		delete(it->second);
	}
	_channels.clear();
	std::map<int, Client*>::iterator c_it = _clients.begin();
	for (; c_it != _clients.end(); ++c_it)
	{
		delete(c_it->second);
		close(c_it->first);
	}
	_clients.clear(); // need to verify with client destructor
	if (_fdSocket >= 0)
		close (_fdSocket);
	_fdSocket = -1;
}

void Server::confServer()
{
	// std::signal(SIGPIPE, SIG_IGN);
	_fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_fdSocket == FAIL)
		sysError(ERR_SOCKET);

	struct sockaddr_in svAddr;
	std::memset(&svAddr, 0, sizeof(svAddr));
	svAddr.sin_family = AF_INET;
	svAddr.sin_addr.s_addr = INADDR_ANY;
	svAddr.sin_port = htons(_port); // 생성자에서 port 받을텐데

	int activeReuse = 1;
	if (setsockopt(_fdSocket, SOL_SOCKET, SO_REUSEADDR, &activeReuse, sizeof(activeReuse)) < 0)
		sysError(ERR_SOCKET); // garantee reuse of the same socket even when server turned off and then on for bind()
	if (fcntl(_fdSocket, F_SETFL, O_NONBLOCK) == -1)
		sysError(ERR_SOCKET);
	int bound = bind(_fdSocket, (struct sockaddr *)&svAddr, sizeof(svAddr)); // give the socket fd to local addr, cast for bind
	if (bound == FAIL)
		sysError(ERR_BIND);

	int listening = listen(_fdSocket, SOMAXCONN); // client can connect() from now on
	if (listening == FAIL)
		sysError(ERR_LISTEN);

	// * update server's fd to the poll list for the idx 0
	struct pollfd temp_pollfd;
	temp_pollfd.fd = _fdSocket;
	temp_pollfd.events = POLLIN;
	temp_pollfd.revents = 0;
	
	std::vector<struct pollfd>::iterator it = _polling.begin();
	_polling.insert(it, temp_pollfd);
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

// void Server::debug_runServer(int flag)
// {
// 	if (flag == 1)
// 	{
// 		std::cout << "this is server fd: " << _fdSocket << std::endl;
// 		std::cout << "all the _polling 'fd' inc server fd on [0]: " << std::endl;
// 		std::vector<struct pollfd>::iterator it = _polling.begin();
// 		int idx = 0;
// 		while (it != _polling.end())
// 		{
// 			std::cout << "fd idx: " << idx << ": " << it->fd << std::endl;
// 			++it;
// 			++idx;
// 		}
// 	}
// 	if (flag == 2)
// 	{
// 		std::cout << "all the _polling 'revent flag' inc server on [0]: " << std::endl;
// 		std::vector<struct pollfd>::iterator it = _polling.begin();
// 		int idx = 0;
// 		while (it != _polling.end())
// 		{
// 			std::string revent_flag;
// 			if (it->revents == POLLIN)
// 				revent_flag = "POLLIN";
// 			if (it->revents == POLLOUT)
// 				revent_flag = "POLLOUT";
// 			if (it->revents == POLLHUP)
// 				revent_flag = "POLLHUP";
// 			std::cout << "revent idx: " << idx << ": " << revent_flag << std::endl;
// 			++it;
// 			++idx;
// 		}
// 	}
// }

void Server::runServer()
{
	while (!sigFlag)
	{
		int fdPerform = poll(_polling.data(), _polling.size(), 1000);
		if (fdPerform < 0)
		{
			if (errno == EINTR)
				continue;
			sysError(ERR_POLL);
		}
		if (fdPerform == 0)
			continue ;
		if (_polling[0].revents & POLLIN) // revents is bitmask so &
			acceptNewClient();
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

// a revoir pour une execution plus coherente en utilisant le flag de la part exec et
// gerer ses flag de struct pollfd dans la partie server
void Server::setPolling(int fd, int flag)
{
	// caller to put flag or i can divide with more evident name for each case
	for (std::vector<struct pollfd>::iterator it = _polling.begin() + 1; it != _polling.end(); ++it)
	{
        if (it->fd == fd)
        {
            if (flag == set_POLLIN)
			    it->events = POLLIN;
            else if (flag == set_POLLOUT)
			    it->events = POLLOUT;
            else if (flag == set_POLLHUP)
                it->events = POLLHUP;
        }
    }
}
