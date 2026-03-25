#include <iostream>
#include "Client.hpp"
#include "Channel.hpp"

void printTestResult(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << "[\033[32mOK\033[0m] " << testName << std::endl;
    } else {
        std::cout << "[\033[31mKO\033[0m] " << testName << std::endl;
    }
}

void testClientLogic() {
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

void testChannelLogic() {
    std::cout << "\n--- TESTS LOGIQUE CHANNEL ---" << std::endl;
    
    Client sender(5, "192.168.1.10");
    Client receiver1(6, "192.168.1.11");
    Client receiver2(7, "192.168.1.12");

    Channel chan("#general");

    chan.addClient(&sender);
    chan.addClient(&receiver1);
    chan.addClient(&receiver2);
    
    printTestResult("Vérification isMember", chan.isMember(&receiver1) == true);

    chan.setInviteOnly(true);
    printTestResult("Application du mode +i", chan.isInviteOnly() == true);

    std::string testMessage = ":sender!user@192.168.1.10 PRIVMSG #general :Bonjour tout le monde\r\n";
    chan.broadcastMessage(testMessage, &sender);

    printTestResult("L'expéditeur ne reçoit pas son propre message", sender.getWriteBuffer() == "");
    printTestResult("Le destinataire 1 reçoit le message", receiver1.getWriteBuffer() == testMessage);
    printTestResult("Le destinataire 2 reçoit le message", receiver2.getWriteBuffer() == testMessage);
}

int main() {
    std::cout << "Démarrage des tests unitaires ft_irc...\n" << std::endl;
    
    testClientLogic();
    testChannelLogic();
    
    std::cout << "\nFin des tests." << std::endl;
    return 0;
}