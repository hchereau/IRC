#include "tests.hpp"
#include "Executor.hpp"
#include "Client.hpp"
#include "Parsing.hpp"
#include <iostream>

#include <iostream>
#include "tests.hpp"
#include "Executor.hpp"
#include "Client.hpp"
#include "Parsing.hpp"

void testDispatcherOnly() {
    std::cout << "\n--- TESTS DISPATCHER (ROUTAGE) ---" << std::endl;
    
    Executor exec(NULL);
    Client client(1, "127.0.0.1");

    Message msgEmpty;
    msgEmpty.cmd = "";
    exec.dispatchMessage(&client, msgEmpty);
    printTestResult("empty command without crash", true); 

    Message msgUnknown;
    msgUnknown.cmd = "INVALIDCMD";
    exec.dispatchMessage(&client, msgUnknown);
    printTestResult("unknow command", true);

    Message msgLower;
    msgLower.cmd = "join";
    exec.dispatchMessage(&client, msgLower);
    printTestResult("command not in upper cases", true);

	// Note AI review:
    // Note pour plus tard : Lorsque tu auras finalisé ton Out Buffer, 
    // tu pourras ajouter un test ici pour vérifier que la commande inconnue 
    // a bien déclenché un appel à Reply::error qui ajoute '421' (ERR_UNKNOWNCOMMAND)
    // dans le buffer du client.
}