#pragma once

#include <string>
#include <sys/socket.h>

// client registration
enum RegistrationState {
    UNREGISTERED,       // Just logged in, nothing sent
    PASS_ACCEPTED,      // serveur password accepted
    NICK_SET,      		// Nick set
    USER_SET,           // USER define
    REGISTERED          // ready to chat
};

class Client {
	public:
	//default
		Client(int fd, const std::string& hostname);
		Client(Client const & other);
		Client& operator=(Client const & other);
		~Client();

	//getters
		int					getFd() const;
		const std::string& 	getNickname() const;
		const std::string& 	getUsername() const;
		const std::string& 	getRealname() const;
		const std::string& 	getHostname() const;
		RegistrationState	getState() const;
		bool isToDisconnect() const;

	//setters
		void	setNickname(const std::string& nick);
		void	setUsername(const std::string& user);
		void	setRealname(const std::string& real);
		void	setState(RegistrationState state);
		void 	setToDisconnect(bool status);

	//buffer management
		void	appendToReadBuffer(const std::string& data);
		void	appendToWriteBuffer(const std::string& data);
		
		std::string	extractMessage();
		
		std::string& getWriteBuffer();
		void clearSentBytes(size_t sentBytes);

	private:
		Client();

		int					_clientFd;
		std::string			_hostname; // Client IP adress

		RegistrationState	_state;

		std::string			_readBuffer;
		std::string			_writeBuffer;

		// hostmark Nickname!Username@Hostname
		std::string			_Nickname;
		std::string			_Username;
		std::string			_Realname;

		bool _toDisconnect;
};