#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

void Executor::execPart(Client *client, const Message&msg){

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
        if (!(_server->chanExists(chanName))){
            Reply::error(client, ERR_NOSUCHCHANNEL, chanName, "No such channel (Channels not implemented yet)");
            continue;
        }
        Channel *channel = _server->getChannelByName(chanName);
        if (channel != NULL){
            if (!(channel->isMember(client))){
                Reply::error(client, ERR_NOTONCHANNEL, chanName, "You're not on that channel");
                continue;
            }
            std::string partMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART :" + chanName + "\r\n";
            channel->broadcastMessage(partMsg, NULL);
            channel->removeClient(client);
        }
    }
}
