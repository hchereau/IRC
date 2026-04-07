#include "Channel.hpp"
#include <algorithm>

//default
Channel::Channel(const std::string& name) 
    : _name(name), _topic(""), _inviteOnly(false), _topicRestricted(false), _key(""), _limit(0) {
}

Channel::~Channel() {}

//setters/getters
const std::string& Channel::getName() const {
    return _name;
}

const std::string& Channel::getTopic() const {
    return _topic;
}

void Channel::setTopic(const std::string& topic) {
    _topic = topic;
}

//Client management
void Channel::addClient(Client* client) {
    if (!isMember(client)) {
        _members.push_back(client);
    }
}

void Channel::removeClient(Client* client) {
    std::vector<Client*>::iterator it = std::find(_members.begin(), _members.end(), client);
    if (it != _members.end()) {
        _members.erase(it);
    } 
    removeOperator(client);
}

bool Channel::isMember(Client* client) const {
    return std::find(_members.begin(), _members.end(), client) != _members.end();
}

const std::vector<Client*>& Channel::getMembers() const
{
    return _members;
}


// operators
void Channel::addOperator(Client* client) {
    if (!isOperator(client)) {
        _operators.push_back(client);
    }
}

void Channel::removeOperator(Client* client) {
    std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
    if (it != _operators.end()) {
        _operators.erase(it);
    }
}

bool Channel::isOperator(Client* client) const {
    return std::find(_operators.begin(), _operators.end(), client) != _operators.end();
}

// modes
// i
bool Channel::isInviteOnly() const { return _inviteOnly; }
void Channel::setInviteOnly(bool i) { _inviteOnly = i; }
// t
bool Channel::isTopicRestricted() const { return _topicRestricted; }
void Channel::setTopicRestricted(bool t) { _topicRestricted = t; }
// k
bool Channel::hasKey() const { return !_key.empty(); }
const std::string& Channel::getKey() const { return _key; }
void Channel::setKey(const std::string& key) { _key = key; }
// l
bool Channel::hasLimit() const { return _limit > 0; }
unsigned int Channel::getLimit() const { return _limit; }
void Channel::setLimit(unsigned int limit) { _limit = limit; } // 0 disables the limit


void Channel::broadcastMessage(const std::string& message, Client* sender) {
    for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it) {
        if (*it != sender) {
            (*it)->appendToWriteBuffer(message);
        }
    }
}

bool Channel::isEmpty() const {
    return _members.empty();
}

void Channel::addInvite(Client* client) {
    _invitedUsers.insert(client);
}

void Channel::removeInvite(Client* client) {
    _invitedUsers.erase(client);
}

bool Channel::isInvited(Client* client) const {
    return _invitedUsers.find(client) != _invitedUsers.end();
}