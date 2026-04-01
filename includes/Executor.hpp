#pragma once

#include <map>
#include <string>
#include "Client.hpp"
#include "Parsing.hpp"

#define ERR_UNKNOWNCOMMAND  421
#define ERR_NOSUCHNICK      401
#define ERR_NOSUCHCHANNEL   403
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_NICKNAMEINUSE   433
#define ERR_NEEDMOREPARAMS  461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH  464

class Server;
class Executor;

typedef void (Executor::*CommandFunction)(Client* client, const Message& msg);

class Executor {
    private:
        Server* _server;
        std::map<std::string, CommandFunction> _commandMap;

        // Toutes ces fonctions sont internes à l'Executor, donc privées
        void execJoin(Client* client, const Message& msg);
        void execNick(Client* client, const Message& msg);
        void execUser(Client* client, const Message& msg);
        void execPass(Client* client, const Message& msg);

    public:
        Executor(Server* server);
        ~Executor();

        // Seul le dispatcher est public pour être appelé par le Server
        void dispatchMessage(Client* client, const Message& msg);
};
