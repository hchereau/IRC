#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Replies.hpp"

static bool validateKickParams(Client* client, const Message& msg) {
    if (msg.params.size() < 2) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "KICK", "Not enough parameters");
        return false;
    }
    return true;
}

static Channel* getValidChannel(Server* server, Client* client, const std::string& channelName) {
    Channel* channel = server->getChannelByName(channelName);
    if (!channel) {
        Reply::error(client, ERR_NOSUCHCHANNEL, "KICK", channelName + " :No such channel");
        return NULL;
    }
    return channel;
}

static bool validateKickerPrivileges(Client* client, Channel* channel) {
    if (!channel->isMember(client)) {
        Reply::error(client, ERR_NOTONCHANNEL, "KICK", channel->getName() + " :You're not on that channel");
        return false;
    }
    if (!channel->isOperator(client)) {
        Reply::error(client, ERR_CHANOPRIVSNEEDED, "KICK", channel->getName() + " :You're not channel operator");
        return false;
    }
    return true;
}

static Client* getValidTarget(Server* server, Client* client, Channel* channel, const std::string& targetNick) {
    Client* target = server->getClientByNick(targetNick);
    if (!target || !channel->isMember(target)) {
        Reply::error(client, ERR_USERNOTINCHANNEL, "KICK", targetNick + " " + channel->getName() + " :They aren't on that channel");
        return NULL;
    }
    return target;
}

static std::string buildKickMessage(Client* kicker, const std::string& channelName, const std::string& targetNick, const std::string& reason) {
    std::string prefix = kicker->getNickname() + "!" + kicker->getUsername() + "@" + kicker->getHostname();
    return ":" + prefix + " KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";
}

void Executor::execKick(Client* client, const Message& msg) {
    if (!validateKickParams(client, msg)) return;

    std::string channelName = msg.params[0];
    std::string targetNick = msg.params[1];    
    std::string reason = msg.trailing.empty() ? "Kicked by channel operator" : msg.trailing;

    Channel* channel = getValidChannel(_server, client, channelName);
    if (!channel) return;
	
    if (!validateKickerPrivileges(client, channel)) return;

    Client* target = getValidTarget(_server, client, channel, targetNick);
    if (!target) return; 
    if (!channel->isOperator(client)) {
        Reply::error(client, ERR_CHANOPRIVSNEEDED, channelName, "You're not channel operator");
        return;
    }

    std::string kickMsg = buildKickMessage(client, channelName, targetNick, reason);
    channel->broadcastMessage(kickMsg, NULL); 

    channel->removeClient(target);
    if (channel->isOperator(target)) {
        channel->removeOperator(target);
    }

    if (channel->isEmpty()) {
        _server->removeChannel(channelName);
    }
}