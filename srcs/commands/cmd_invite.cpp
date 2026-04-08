#include "Executor.hpp"
#include "Server.hpp"
#include "CommandValidator.hpp"
#include "Replies.hpp"
#include "Channel.hpp"

void Executor::execInvite(Client* client, const Message& msg) {
    if (!CommandValidator::hasMinParams(msg.params, 2)) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "INVITE", "Not enough parameters");
        return;
    }

    std::string targetNick = msg.params[0];
    std::string channelName = msg.params[1];

    Client* targetClient = _server->getClientByNick(targetNick);
    if (!targetClient) {
        Reply::error(client, ERR_NOSUCHNICK, targetNick, "No such nick/channel");
        return;
    }

    Channel* channel = _server->getChannelByName(channelName);
    if (!channel) {
        Reply::error(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
        return;
    }

    if (!channel->isMember(client)) {
        Reply::error(client, ERR_NOTONCHANNEL, channelName, "You're not on that channel");
        return;
    }

    if (channel->isMember(targetClient)) {
        // La RFC demande le format : "<user> <channel> :is already on channel"
        Reply::error(client, ERR_USERONCHANNEL, targetNick + " " + channelName, "is already on channel");
        return;
    }

    if (channel->isInviteOnly()) {
        if (!channel->isOperator(client)) {
            Reply::error(client, ERR_CHANOPRIVSNEEDED, channel->getName(), "You're not channel operator");
            return;
        }
    }

    channel->addInvite(targetClient);

    Reply::custom(client, RPL_INVITING, targetNick + " " + channelName);

    std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
    std::string inviteMsg = prefix + " INVITE " + targetNick + " :" + channelName + "\r\n";
    targetClient->appendToWriteBuffer(inviteMsg);
}