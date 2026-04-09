#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

void Executor::execNick(Client* client, const Message& msg) {
    if (!CommandValidator::hasMinParams(msg.params, 1) || msg.params[0].empty()) {
        Reply::error(client, ERR_NONICKNAMEGIVEN, "NICK", "No nickname given");
        return;
    }

    std::string newNick = msg.params[0];

    if (!CommandValidator::isValidNickname(newNick)) {
        Reply::error(client, ERR_ERRONEUSNICKNAME, newNick, "Erroneous nickname");
        return;
    }

    Client* existingClient = _server->getClientByNick(newNick);
    if (existingClient != NULL && existingClient != client) {
        Reply::error(client, ERR_NICKNAMEINUSE, newNick, "Nickname is already in use");
        return;
    }
    

    client->setNickname(newNick);

    checkRegistration(client);
}