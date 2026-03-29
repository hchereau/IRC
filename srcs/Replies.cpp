#include "Replies.hpp"

void Reply::error(Client* client, int code, const std::string& command, const std::string& msg) {
    std::stringstream ss;
    std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
    
    ss << ":ft_irc " << code << " " << nick << " " << command << " :" << msg << "\r\n";
    client->appendToWriteBuffer(ss.str());
}

void Reply::welcome(Client* client) {
    std::stringstream ss;
    ss << ":ft_irc " << RPL_WELCOME << " " << client->getNickname() 
       << " :Welcome to the ft_irc Network, " << client->getNickname() 
       << "!" << client->getUsername() << "@" << "localhost\r\n";
    
    client->appendToWriteBuffer(ss.str());
}

void Reply::raw(Client* client, const std::string& rawMsg) {
    client->appendToWriteBuffer(rawMsg + "\r\n");
}