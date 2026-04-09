#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

static std::string buildPartMessage(Client* parted, const std::string& chanName, const std::string& reason) {
    std::string prefix = parted->getNickname() + "!" + parted->getUsername() + "@" + parted->getHostname();
    return ":" + prefix + " PART " + chanName + " :" + reason + "\r\n";
}

void Executor::execPart(Client *client, const Message&msg){

    // std::cout << "[DEBUG] execPart called with: " << msg.params[0] << std::endl; //debug

    if (!CommandValidator::hasMinParams(msg.params, 1) || msg.params[0].empty()){
        Reply::error(client, ERR_NEEDMOREPARAMS, "PART", "Not enough parameters");
        return;
    }

    std::vector<std::string> channels = split(msg.params[0], ',');
    for (size_t i = 0; i < channels.size(); ++i){
        std::string chanName = channels[i];
        if (!CommandValidator::isValidChannelName(chanName)) {
            Reply::error(client, ERR_NOSUCHCHANNEL, chanName, "No such channel");
            continue;
        }
        Channel *channel = _server->getChannelByName(chanName);
        if (channel == NULL){
            // std::cout << "[DEBUG] no such channel: " << chanName << std::endl; // debug
            Reply::error(client, ERR_NOSUCHCHANNEL, chanName, "No such channel (Channels not implemented yet)");
            continue;
        }
        if (!(channel->isMember(client))){
            Reply::error(client, ERR_NOTONCHANNEL, chanName, "You're not on that channel");
            continue;
        }
        std::string partMsg;
        std::string reason = msg.trailing;
        if (reason.empty()){
            partMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART " + chanName + "\r\n";
        }
        else {
            partMsg = buildPartMessage(client, chanName, reason);
        }
        channel->broadcastMessage(partMsg, NULL);
        channel->removeClient(client);
        if (channel->isEmpty()) {
            _server->removeChannel(chanName);
        }
    }
}
