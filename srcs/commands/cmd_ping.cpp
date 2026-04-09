#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

void Executor::execPing(Client* client, const Message& msg) {
    if (!CommandValidator::hasMinParams(msg.params, 1) || msg.params[0].empty()) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "PING", "Not enough parameters");
        return;
    }
    std::string token = msg.params[0];
    std::string pongResponse = ":ft_irc PONG ft_irc :" + token + "\r\n"; 
    client->appendToWriteBuffer(pongResponse);
}