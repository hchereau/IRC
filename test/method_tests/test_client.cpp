#include <iostream>
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