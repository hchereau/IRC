#include "Executor.hpp"
#include "CommandValidator.hpp"
#include "Replies.hpp"
#include <sstream>
#include "Server.hpp"

Executor::Executor(Server* server) : _server(server) {
    _commandMap["JOIN"] = &Executor::execJoin;
    _commandMap["NICK"] = &Executor::execNick;
    _commandMap["PASS"] = &Executor::execPass;
    _commandMap["USER"] = &Executor::execUser;
}

Executor::~Executor() {}

void Executor::dispatchMessage(Client* client, const Message& msg) {
    if (msg.cmd.empty())
        return;

    std::map<std::string, CommandFunction>::iterator it = _commandMap.find(msg.cmd);

    if (it != _commandMap.end()) {
        CommandFunction func = it->second;
        (this->*func)(client, msg);
    } else {
        Reply::error(client, ERR_UNKNOWNCOMMAND, msg.cmd, "Unknown command");
    }
}

void Executor::execJoin(Client* client, const Message& msg) {
    if (!CommandValidator::hasMinParams(msg.params, 1)) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
        return;
    }

    std::string channelName = msg.params[0];

    if (!CommandValidator::isValidChannelName(channelName)) {
        Reply::error(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
        return;
    }

    // TODO: Implémenter la suite avec _server->getOrCreateChannel(channelName)
}

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

void Executor::execUser(Client* client, const Message& msg) {
    if (client->getState() == REGISTERED) {
        Reply::error(client, ERR_ALREADYREGISTRED, "USER", "Unauthorized command (already registered)");
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

void Executor::checkRegistration(Client* client) {
    if (client->getState() == REGISTERED) {
        return;
    }

    if (!_server->getPassword().empty() && client->getState() < PASS_ACCEPTED) {
        return; 
    }

    // Vérifie si NICK et USER sont remplis
    if (!client->getNickname().empty() && !client->getUsername().empty()) {
        client->setState(REGISTERED);
        Reply::welcome(client); // Envoie le 001 RPL_WELCOME
    }
}