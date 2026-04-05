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
    _commandMap["PRIVMSG"] = &Executor::execPrivmsg;
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


