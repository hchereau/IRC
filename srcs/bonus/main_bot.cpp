#include "bot.hpp"
#include <iostream>
#include <cstdlib>
#include <csignal>

bool g_botRunning = true;

void handle_sigint(int sig) {
    (void)sig;
    g_botRunning = false;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: ./bot <IP> <Port> <Password>" << std::endl;
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    std::string ip = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];

    IrcBot bot(password);

    if (bot.connectToServer(ip, port)) {
        std::cout << "Bot connecté. En attente de commandes..." << std::endl;
        bot.run();
    }

    std::cout << "\nArrêt du bot..." << std::endl;
    return 0;
}