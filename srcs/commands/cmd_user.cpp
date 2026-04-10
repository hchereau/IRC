#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"

void Executor::execUser(Client* client, const Message& msg) {
    if (client->getState() == REGISTERED) {
        Reply::error(client, ERR_ALREADYREGISTRED, "USER", "Unauthorized command (already registered)");
        return;
    }

    if (!_server->getPassword().empty() && client->getState() < PASS_ACCEPTED) {
        Reply::error(client, ERR_PASSWDMISMATCH, "USER", "Password required");
        client->setToDisconnect(true);
        return;
    }

    if (msg.params.size() < 3 || (msg.params.size() == 3 && msg.trailing.empty())) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
        return;
    }

    client->setUsername(msg.params[0]);
    std::string realname = msg.trailing.empty() ? msg.params[3] : msg.trailing;

    client->setState(USER_SET);

    checkRegistration(client);
}