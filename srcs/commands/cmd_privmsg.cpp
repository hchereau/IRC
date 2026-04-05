#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"

void Executor::execPrivmsg(Client* client, const Message& msg) {
    if (client->getState() != REGISTERED) {
        return;
    }

    if (msg.params.empty()) {
        Reply::error(client, ERR_NORECIPIENT, "PRIVMSG", "No recipient given (PRIVMSG)");
        return;
    }

    std::string target = msg.params[0];
    std::string text = extractMessageText(msg);

    if (text.empty()) {
        Reply::error(client, ERR_NOTEXTTOSEND, "", "No text to send");
        return;
    }

    if (target[0] == '#' || target[0] == '&') {
        // TODO: À remplacer plus tard par la logique de Channel
        Reply::error(client, ERR_NOSUCHCHANNEL, target, "No such channel (Channels not implemented yet)");
        return;
    }

    sendPrivateMessage(client, target, text);
}

std::string Executor::extractMessageText(const Message& msg) const {
    if (!msg.trailing.empty()) {
        return msg.trailing;
    } else if (msg.params.size() > 1) {
        return msg.params[1];
    }
    return "";
}

void Executor::sendPrivateMessage(Client* sender, const std::string& targetNick, const std::string& text) {
    Client* targetClient = _server->getClientByNick(targetNick);
    
    if (!targetClient) {
        Reply::error(sender, ERR_NOSUCHNICK, targetNick, "No such nick/channel");
        return;
    }

    // Format : :senderNick!senderUser@senderHost PRIVMSG targetNick :text\r\n
    std::string prefix = ":" + sender->getNickname() + "!" + sender->getUsername() + "@" + sender->getHostname();
    std::string fullMsg = prefix + " PRIVMSG " + targetNick + " :" + text + "\r\n";

    targetClient->appendToWriteBuffer(fullMsg);
}