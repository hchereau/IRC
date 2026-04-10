#include "Executor.hpp"
#include "CommandValidator.hpp"
#include "Replies.hpp"
#include <sstream>
#include "Server.hpp"

Executor::Executor(Server* server) : _server(server) {
    _commandMap["JOIN"] = &Executor::execJoin;
    _commandMap["PART"] = &Executor::execPart;
    _commandMap["NICK"] = &Executor::execNick;
    _commandMap["PASS"] = &Executor::execPass;
    _commandMap["USER"] = &Executor::execUser;
    _commandMap["PRIVMSG"] = &Executor::execPrivmsg;
    _commandMap["KICK"] = &Executor::execKick;
    _commandMap["INVITE"] = &Executor::execInvite;
    _commandMap["MODE"] = &Executor::execMode;
    _commandMap["PING"] = &Executor::execPing;
	_commandMap["TOPIC"] = &Executor::execTopic;
    _commandMap["CAP"] = &Executor::execCap;
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

//irssi
void Executor::execCap(Client* client, const Message& msg) {
    if (!msg.params.empty() && msg.params[0] == "LS") {
        client->appendToWriteBuffer("CAP * LS :\r\n");
    }
}



