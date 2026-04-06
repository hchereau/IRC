#pragma once

#include <map>
#include <string>
#include "Client.hpp"
#include "Channel.hpp"
#include "Parsing.hpp"

class Server;
class Executor;

typedef void (Executor::*CommandFunction)(Client* client, const Message& msg);

class Executor {
    private:
        Server* _server;
        std::map<std::string, CommandFunction> _commandMap;

        // Toutes ces fonctions sont internes à l'Executor, donc privées
        void execNick(Client* client, const Message& msg);
        void execUser(Client* client, const Message& msg);
        void execPass(Client* client, const Message& msg);
        void execPart(Client* client, const Message& msg);
    
        void execPrivmsg(Client* client, const Message& msg);
        std::string extractMessageText(const Message& msg) const;
        void sendPrivateMessage(Client* sender, const std::string& targetNick, const std::string& text);
        void privmsgToChannel(Client* client, const std::string& target, const std::string& text);
        void privmsgToUser(Client* client, const std::string& target, const std::string& text);

        void execJoin(Client* client, const Message& msg);
        void sendNamesReply(Client* client, Channel* channel);

        void execKick(Client* client, const Message& msg);

    public:
        Executor(Server* server);
        ~Executor();

        // Seul le dispatcher est public pour être appelé par le Server
        void dispatchMessage(Client* client, const Message& msg);
        void checkRegistration(Client* client);
};

std::vector<std::string> split(const std::string& str, char delimiter);
