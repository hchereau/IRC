#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void Executor::execJoin(Client* client, const Message& msg) {

    if (!CommandValidator::hasMinParams(msg.params, 1)) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
        return;
    }

    if (client->getState() != REGISTERED) {
        Reply::error(client, ERR_NOTREGISTERED, "JOIN", "You have not registered");
        return;
    }

    std::vector<std::string> channels = split(msg.params[0], ',');
    std::vector<std::string> keys;
    if (msg.params.size() > 1) {
        keys = split(msg.params[1], ',');
    }

    for (size_t i = 0; i < channels.size(); ++i) {
        std::string chanName = channels[i];
        std::string key = (i < keys.size()) ? keys[i] : "";

        if (!CommandValidator::isValidChannelName(chanName)) {
            Reply::error(client, ERR_NOSUCHCHANNEL, chanName, "No such channel");
            continue;
        }

        Channel* channel = _server->getOrCreateChannel(chanName);

        if (channel->hasKey() && channel->getKey() != key) {
            Reply::error(client, ERR_BADCHANNELKEY, chanName, "Cannot join channel (+k)");
            continue;
        }

        if (channel->hasLimit() && channel->getMembers().size() >= channel->getLimit()) {
            Reply::error(client, ERR_CHANNELISFULL, chanName, "Cannot join channel (+l)");
            continue;
        }

        if (channel->isInviteOnly()) {
            if (!channel->isInvited(client)) {
                Reply::error(client, ERR_INVITEONLYCHAN, chanName, "Cannot join channel (+i)");
                continue;
            }
            channel->removeInvite(client);
        }

        channel->addClient(client);

        if (channel->getMembers().size() == 1) {
            channel->addOperator(client);
        }

        std::string joinMsg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " JOIN :" + chanName + "\r\n";
        channel->broadcastMessage(joinMsg, NULL); // NULL ou sender selon ta logique de broadcast, ici tout le monde doit le recevoir

        if (!channel->getTopic().empty()) {
            std::string topicMsg = ":server 332 " + client->getNickname() + " " + chanName + " :" + channel->getTopic();
            Reply::raw(client, topicMsg);
        }

        sendNamesReply(client, channel);
    }
}

void Executor::sendNamesReply(Client* client, Channel* channel) {
    std::string chanName = channel->getName();
    std::string namesList = "";
    
    const std::vector<Client*>& members = channel->getMembers();
    for (size_t i = 0; i < members.size(); ++i) {
        if (channel->isOperator(members[i])) {
            namesList += "@";
        }
        namesList += members[i]->getNickname();
        
        if (i < members.size() - 1) {
            namesList += " ";
        }
    }
    
    std::ostringstream oss353;
    oss353 << ":server 353 " << client->getNickname() << " = " << chanName << " :" << namesList;
    Reply::raw(client, oss353.str());

    std::ostringstream oss366;
    oss366 << ":server 366 " << client->getNickname() << " " << chanName << " :End of /NAMES list.";
    Reply::raw(client, oss366.str());
}