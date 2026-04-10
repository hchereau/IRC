#include <iostream>
#include <vector>
#include <sys/resource.h>
#include "tests.hpp"
#include "Client.hpp"

void runClientTests() {
    std::cout << "--- TESTS LOGIQUE CLIENT ---" << std::endl;
    
    Client client1(4, "127.0.0.1");
    
    client1.appendToReadBuffer("JOIN #ge");
    std::string msg1 = client1.extractMessage();
    printTestResult("Extraction bloquée si pas de \\r\\n", msg1 == "");
    client1.appendToReadBuffer("neral\r\nPRIVM");
    std::string msg2 = client1.extractMessage();
    printTestResult("Extraction de la première commande complète", msg2 == "JOIN #general");
    client1.appendToReadBuffer("SG #general :salut\r\n");
    std::string msg3 = client1.extractMessage();
    printTestResult("Extraction de la commande en attente", msg3 == "PRIVMSG #general :salut");
}

void test_out_of_memory() {
    std::cout << "\n--- TEST : Gestion Out of Memory (std::bad_alloc) ---" << std::endl;
    
    struct rlimit old_limit;
    getrlimit(RLIMIT_AS, &old_limit);

    struct rlimit new_limit;
    new_limit.rlim_cur = 1024 * 1024 * 10;
    new_limit.rlim_max = old_limit.rlim_max;
    setrlimit(RLIMIT_AS, &new_limit);

    std::vector<Client*> test_clients;
    bool memory_limit_reached = false;
    
    for (int i = 0; i < 50000; ++i) {
        Client* c = new (std::nothrow) Client(i, "test.local");
        
        if (c == NULL) {
            memory_limit_reached = true;
            std::cout << "[\033[32mOK\033[0m] Le système a renvoyé NULL au lieu de crasher !" << std::endl;
            break;
        }
        test_clients.push_back(c);
    }

    if (!memory_limit_reached) {
        std::cout << "[WARNING] La limite de RAM n'a pas été atteinte. Augmente la taille de la boucle." << std::endl;
    }

    for (size_t j = 0; j < test_clients.size(); ++j) {
        delete test_clients[j];
    }
    setrlimit(RLIMIT_AS, &old_limit);
}