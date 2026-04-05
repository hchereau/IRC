#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

void Executor::execPass(Client* client, const Message& msg) {
    if (client->getState() == REGISTERED) {
        Reply::error(client, ERR_ALREADYREGISTRED, "PASS", "Unauthorized command (already registered)");
        return;
    }

    if (!CommandValidator::hasMinParams(msg.params, 1) || msg.params[0].empty()) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "PASS", "Not enough parameters");
        return;
    }
    
    if (msg.params[0] != _server->getPassword()) {
        Reply::error(client, ERR_PASSWDMISMATCH, "PASS", "Password incorrect");
        client->setToDisconnect(true); 
        return;
    }

    client->setState(PASS_ACCEPTED);
}