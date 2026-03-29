#include "Executor.hpp"
#include "CommandValidator.hpp"
#include "Replies.hpp"
#include <sstream>

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

    // faut vérifier si le mdp est valid ici avec  _server->getPassword()

    if (!CommandValidator::isValidNickname(newNick)) {
        Reply::error(client, ERR_ERRONEUSNICKNAME, newNick, "Erroneous nickname");
        return;
    }

    // TODO: Vérifier isNicknameTaken avec le Server

    client->setNickname(newNick);

    if (client->getState() == PASS_ACCEPTED) {
        client->setState(NICK_SET);
    }
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

    std::string serverPassword = "test"; // mdp en dur pour le moment. Il faut faire _server->getPassword() (on attends le serveur pour l'instant) 
    
    if (msg.params[0] != serverPassword) {
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

    if (!CommandValidator::hasMinParams(msg.params, 4)) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "USER", "Not enough parameters");
        return;
    }

    client->setUsername(msg.params[0]);
    // C'est commun de ne pas utiliser 1 et 2 dnas les serveurs pro
    client->setRealname(msg.params[3]);

    client->setState(USER_SET);

    if (client->getNickname() != "") {
        client->setState(REGISTERED);
        
        Reply::welcome(client);
    }
}