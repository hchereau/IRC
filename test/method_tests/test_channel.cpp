#include "tests.hpp"
#include "Channel.hpp"
#include <iostream>

void runChannelTests() {
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