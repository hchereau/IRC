#include "Client.hpp"

Client::Client(int fd, const std::string& hostname) 
    : _clientFd(fd), _hostname(hostname), _state(UNREGISTERED) {
}

Client::Client(Client const & other) {
    *this = other;
}

Client& Client::operator=(Client const & other) {
    if (this != &other) {
        this->_clientFd = other._clientFd;
        this->_hostname = other._hostname;
        this->_state = other._state;
        this->_readBuffer = other._readBuffer;
        this->_writeBuffer = other._writeBuffer;
        this->_Nickname = other._Nickname;
        this->_Username = other._Username;
        this->_Realname = other._Realname;
    }
    return *this;
}

Client::~Client() {}

//getters
int Client::getFd() const {
    return _clientFd;
}

const std::string& Client::getNickname() const {
    return _Nickname;
}

const std::string& Client::getUsername() const {
    return _Username;
}

const std::string& Client::getRealname() const {
    return _Realname;
}

const std::string& Client::getHostname() const {
    return _hostname;
}

RegistrationState Client::getState() const {
    return _state;
}

//setters
void Client::setNickname(const std::string& nick) {
    _Nickname = nick;
}

void Client::setUsername(const std::string& user) {
    _Username = user;
}

void Client::setRealname(const std::string& real) {
    _Realname = real;
}

void Client::setState(RegistrationState state) {
    _state = state;
}

// buffer management
void Client::appendToReadBuffer(const std::string& data) {
    _readBuffer += data;
}

void Client::appendToWriteBuffer(const std::string& data) {
    _writeBuffer += data;
}

std::string& Client::getWriteBuffer() {
    return _writeBuffer;
}

void Client::clearWriteBuffer() {
    _writeBuffer.clear();
}

std::string Client::extractMessage() {
    std::string message = "";
    size_t pos = _readBuffer.find("\r\n"); // \r\n is the end-of-line process

    if (pos != std::string::npos) {
        message = _readBuffer.substr(0, pos);
        _readBuffer.erase(0, pos + 2); // rerase \r\n
    }
    return message;
}