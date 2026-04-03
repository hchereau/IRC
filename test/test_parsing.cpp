#include <iostream>
#include "tests.hpp"
#include "Parsing.hpp"

void runParsingTests() {
    std::cout << "\n--- TESTS PARSING ---" << std::endl;
    
    Parsing parser;
    Message msg;

    // 1. Commande basique sans préfixe ni trailing
    msg = parser.parseLine("NICK toto");
    printTestResult("Basic - Command", msg.cmd == "NICK");
    printTestResult("Basic - Params", msg.params.size() == 1 && msg.params[0] == "toto");

    // 2. Commande avec Trailing contenant de multiples espaces
    msg = parser.parseLine("QUIT :    you should keep the      spaces        ");
    printTestResult("Trailing - Spaces", msg.cmd == "QUIT" && msg.trailing == "    you should keep the      spaces        ");

    // 3. Message complet (Préfixe + Commande + Paramètre + Trailing)
    msg = parser.parseLine(":nick!user@host PRIVMSG #chan :hello world");
    printTestResult("Prefix - Extraction", msg.prefix == "nick!user@host");
    printTestResult("Prefix - Command", msg.cmd == "PRIVMSG");
    printTestResult("Prefix - Params", msg.params.size() == 1 && msg.params[0] == "#chan");
    printTestResult("Prefix - Trailing", msg.trailing == "hello world");

    // 4. FIX DU BUG : ':' au milieu d'un paramètre
    msg = parser.parseLine("USER toto:test 0 * :Toto User");
    printTestResult("Bug Fix - Command", msg.cmd == "USER");
    printTestResult("Bug Fix - Params Size", msg.params.size() == 3);
    printTestResult("Bug Fix - Param with colon", msg.params[0] == "toto:test");
    printTestResult("Bug Fix - Trailing", msg.trailing == "Toto User");

    // 5. Multiples ':' dans le trailing
    msg = parser.parseLine("PRIVMSG #chan :hello :world");
    printTestResult("Double Colon - Trailing", msg.trailing == "hello :world");

    // 6. Trailing vide
    msg = parser.parseLine("PRIVMSG #chan :");
    printTestResult("Empty Trailing", msg.cmd == "PRIVMSG" && msg.trailing == "");

    // 7. Cas limite : Message vide
    msg = parser.parseLine("");
    printTestResult("Empty Message", msg.cmd == "" && msg.prefix == "" && msg.params.empty());

    // 8. Cas limite : Que des espaces
    msg = parser.parseLine("      ");
    printTestResult("Only spaces", msg.cmd == "" && msg.prefix == "" && msg.params.empty());

	// 9. Cas limite : Dépassement de la taille maximale (510 caractères sans \r\n)
    // On crée un message qui fait volontairement plus de 600 caractères
    std::string longMessage = "PRIVMSG #chan :";
    longMessage.append(600, 'A'); // Ajoute 600 'A'
    msg = parser.parseLine(longMessage);
    
    // Le message doit être tronqué à 510 caractères au total.
    // "PRIVMSG #chan :" fait 15 caractères. Le trailing tronqué doit donc faire (510 - 15) = 495 caractères.
    printTestResult("Max Length - Truncation", msg.cmd == "PRIVMSG" && msg.trailing.length() == 495);

    // 10. Commande seule sans paramètre ni trailing
    msg = parser.parseLine("QUIT");
    printTestResult("Command Only", msg.cmd == "QUIT" && msg.params.empty() && msg.trailing.empty());
}