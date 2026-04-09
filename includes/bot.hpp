#ifndef BOT_HPP
#define BOT_HPP

#include <string>

class IrcBot {
private:
    int _botSocket;
    std::string _serverPassword;

    void sendData(const std::string& data);
    void fireMissile(const std::string& target); // Notre nouvelle fonction

public:
    IrcBot(const std::string& pass);
    ~IrcBot();
    
    bool connectToServer(const std::string& ip, int port);
    void run();
    void parseAndReact(const std::string& message);
};

#endif