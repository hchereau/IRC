#include "bot.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>

IrcBot::IrcBot(const std::string& pass) : _serverPassword(pass) {
    srand(time(NULL)); 
}

IrcBot::~IrcBot() { close(_botSocket); }

void IrcBot::sendData(const std::string& data) {
    send(_botSocket, data.c_str(), data.length(), 0);
}

bool IrcBot::connectToServer(const std::string& ip, int port) {
    _botSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(_botSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Erreur de connexion au serveur IRC" << std::endl;
        return false;
    }

    sendData("PASS " + _serverPassword + "\r\n");
    sendData("NICK Kuru_bot\r\n");
    sendData("USER Kuru_bot 0 * :Kuru_bot 3000\r\n");

	sendData("JOIN #general\r\n");
    
    return true;
}

void IrcBot::run() {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(_botSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break; 

        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        
        parseAndReact(message);
    }
}

void IrcBot::fireMissile(const std::string& target) {
    std::string missiles[] = {
        "balistique", "nucléaire tactique", "à tête chercheuse",
        "en mousse", "intercontinental", "chargé de confettis"
    };

    int numMissiles = sizeof(missiles) / sizeof(missiles[0]);
    int randomIndex = rand() % numMissiles;

    std::string type = missiles[randomIndex];
    std::string msg = "PRIVMSG " + target + " :Bip boop... Lancement d'un missile " + type + " en cours !\r\n";
    
    sendData(msg);
}

void IrcBot::parseAndReact(const std::string& message) {
    if (message.find("PRIVMSG") != std::string::npos && message.find("!missile") != std::string::npos) {
        
        size_t privmsgPos = message.find("PRIVMSG ");
        if (privmsgPos != std::string::npos) {
            size_t targetStart = privmsgPos + 8;
            size_t targetEnd = message.find(" ", targetStart);
            
            if (targetEnd != std::string::npos) {
                std::string target = message.substr(targetStart, targetEnd - targetStart);
                fireMissile(target);
            }
        }
    }
}