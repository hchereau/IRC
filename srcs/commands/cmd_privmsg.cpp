#include "Executor.hpp"
#include "Server.hpp"
#include "CommandValidator.hpp"
#include "Replies.hpp"
#include "Channel.hpp"

// 1. La fonction principale agit comme un routeur
void Executor::execPrivmsg(Client* client, const Message& msg) {
    // 1. Vérifier le destinataire
    if (!CommandValidator::hasMinParams(msg.params, 1)) {
        Reply::error(client, ERR_NORECIPIENT, "PRIVMSG", "No recipient given");
        return;
    }

    // 2. Extraire le texte (soit du trailing, soit du params[1])
    std::string text = "";
    if (!msg.trailing.empty()) {
        text = msg.trailing;
    } else if (msg.params.size() > 1) {
        text = msg.params[1];
    }

    // 3. Vérifier si on a du texte à envoyer
    if (text.empty()) {
        Reply::error(client, ERR_NOTEXTTOSEND, "", "No text to send");
        return;
    }

    std::string target = msg.params[0];

    // 4. Routage selon le préfixe
    if (target[0] == '#' || target[0] == '&') {
        privmsgToChannel(client, target, text);
    } else {
        privmsgToUser(client, target, text);
    }
}

// 2. Logique dédiée aux Canaux
void Executor::privmsgToChannel(Client* client, const std::string& target, const std::string& text) {
    Channel* channel = _server->getChannelByName(target); // Suppose que Server::getChannel existe et renvoie NULL si introuvable
    
    if (!channel) {
        Reply::error(client, ERR_NOSUCHCHANNEL, target, "No such channel");
        return;
    }

    if (!channel->isMember(client)) {
        Reply::error(client, ERR_CANNOTSENDTOCHAN, target, "Cannot send to channel");
        return;
    }

    std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
    std::string fullMsg = prefix + " PRIVMSG " + target + " :" + text + "\r\n";
    
    // Broadcast au canal (en excluant l'expéditeur grâce au 2ème paramètre)
    channel->broadcastMessage(fullMsg, client);
}

// 3. Logique dédiée aux Utilisateurs
void Executor::privmsgToUser(Client* client, const std::string& target, const std::string& text) {
    Client* targetClient = _server->getClientByNick(target);
    
    if (!targetClient) {
        Reply::error(client, ERR_NOSUCHNICK, target, "No such nick/channel");
        return;
    }

    std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
    std::string fullMsg = prefix + " PRIVMSG " + target + " :" + text + "\r\n";
    
    Reply::raw(targetClient, fullMsg);
}