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

    if (!_server->getPassword().empty() && client->getState() < PASS_ACCEPTED) {
        Reply::error(client, ERR_PASSWDMISMATCH, "USER", "Password required");
        client->setToDisconnect(true);
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
    
    std::string oldNick = client->getNickname();
    std::string oldPrefix = oldNick.empty() ? "" : (oldNick + "!" + client->getUsername() + "@" + client->getHostname());
    client->setNickname(newNick);
    // client->appendToWriteBuffer(nickNotification);
    // _server->broadcastToSharedChannels(client, nickNotification);
    if (client->getState() == REGISTERED && !oldPrefix.empty()) {
        std::string nickMsg = ":" + oldPrefix + " NICK :" + newNick + "\r\n";
        client->appendToWriteBuffer(nickMsg);  
        _server->broadcastToSharedChannels(client, nickMsg);
    }

    checkRegistration(client);
}