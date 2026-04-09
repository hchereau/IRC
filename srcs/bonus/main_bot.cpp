#include "bot.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: ./bot <IP> <Port> <Password>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];

    IrcBot bot(password);

    if (bot.connectToServer(ip, port)) {
        std::cout << "Bot connecté. En attente de commandes..." << std::endl;
        bot.run();
    }

    return 0;
}