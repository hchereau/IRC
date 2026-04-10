#include "Executor.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Replies.hpp"
#include <cstdlib>
#include <sstream>

void Executor::execMode(Client* client, const Message& msg) {

    if (client->getState() != REGISTERED) {
        Reply::error(client, ERR_NOTREGISTERED, "MODE", "You have not registered");
        return;
    }

    if (msg.params.empty()) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "MODE", "Not enough parameters");
        return;
    }

    std::string target = msg.params[0];

    if (target.empty() || (target[0] != '#' && target[0] != '&')) {
        return; 
    }

    Channel* channel = _server->getChannelByName(target);
    if (!channel) {
        Reply::error(client, ERR_NOSUCHCHANNEL, target, "No such channel");
        return;
    }

    // Si juste "MODE #channel", on renvoie les modes actuels (324)
    if (msg.params.size() == 1) {
        std::string modes = "+";
        std::string modeParams = "";

        if (channel->isInviteOnly()) modes += "i";
        if (channel->isTopicRestricted()) modes += "t";
        if (channel->hasKey()) modes += "k";
        if (channel->hasLimit()) {
            modes += "l";
            std::ostringstream oss;
            oss << channel->getLimit();
            modeParams += " " + oss.str();
        }

        std::string replyMsg = target + " " + modes + modeParams;
        Reply::custom(client, RPL_CHANNELMODEIS, replyMsg); 
        return;
    }

    if (!channel->isOperator(client)) {
        Reply::error(client, ERR_CHANOPRIVSNEEDED, target, "You're not channel operator");
        return;
    }

    std::string modeString = msg.params[1];
    std::vector<std::string> modeParams;
    if (msg.params.size() > 2) {
        modeParams.assign(msg.params.begin() + 2, msg.params.end());
    }

    processModeString(client, channel, modeString, modeParams);
}

void Executor::processModeString(Client* client, Channel* channel, const std::string& modeString, const std::vector<std::string>& params) {
    bool isAdding = true; 
    size_t paramIndex = 0;
    
    std::string appliedModes = "";
    std::string appliedParams = "";

    for (size_t i = 0; i < modeString.length(); ++i) {
        char c = modeString[i];

        if (c == '+') {
            isAdding = true;
            if (appliedModes.empty() || appliedModes[appliedModes.length()-1] != '+')
                appliedModes += "+";
        } else if (c == '-') {
            isAdding = false;
            if (appliedModes.empty() || appliedModes[appliedModes.length()-1] != '-')
                appliedModes += "-";
        } else {
            bool modeChanged = false;
            std::string param = "";

            if (c == 'o' || c == 'k' || (c == 'l' && isAdding)) {
                if (paramIndex < params.size()) {
                    param = params[paramIndex++];
                } else {
                    continue; // RFC: Paramètre manquant, on ignore ce mode
                }
            }

            switch (c) {
                case 'i': modeChanged = applyModeI(channel, isAdding); break;
                case 't': modeChanged = applyModeT(channel, isAdding); break;
                case 'k': modeChanged = applyModeK(channel, isAdding, param); break;
                case 'l': modeChanged = applyModeL(channel, isAdding, param); break;
                case 'o': 
                    modeChanged = applyModeO(channel, isAdding, param, client); 
                    break;
                default:
                    Reply::error(client, ERR_UNKNOWNMODE, std::string(1, c), "is unknown mode char to me");
                    break;
            }

            if (modeChanged) {
                appliedModes += c;
                if (!param.empty()) {
                    appliedParams += " " + param;
                }
            }
        }
    }

    if (!appliedModes.empty() && (appliedModes[appliedModes.length()-1] == '+' || appliedModes[appliedModes.length()-1] == '-')) {
        appliedModes.erase(appliedModes.length() - 1);
    }

    if (!appliedModes.empty()) {
        std::string broadcastMsg = ":" + client->getNickname() + " MODE " + channel->getName() + " " + appliedModes + appliedParams + "\r\n";
        channel->broadcastMessage(broadcastMsg, NULL);
    }
}

bool Executor::applyModeI(Channel* channel, bool isAdding) {
    if (channel->isInviteOnly() == isAdding) return false;
    channel->setInviteOnly(isAdding);
    return true;
}

bool Executor::applyModeT(Channel* channel, bool isAdding) {
    if (channel->isTopicRestricted() == isAdding) return false;
    channel->setTopicRestricted(isAdding);
    return true;
}

bool Executor::applyModeK(Channel* channel, bool isAdding, const std::string& param) {
    if (isAdding) {
        if (param.empty() || (channel->hasKey() && channel->getKey() == param)) return false;
        channel->setKey(param);
        return true;
    } else {
        if (!channel->hasKey()) return false;
        channel->setKey(""); 
        return true;
    }
}

bool Executor::applyModeL(Channel* channel, bool isAdding, const std::string& param) {
    if (isAdding) {
        int limit = std::atoi(param.c_str());
        if (limit <= 0) return false;
        if (channel->hasLimit() && channel->getLimit() == (unsigned int)limit) return false;
        
        channel->setLimit((unsigned int)limit);
        return true;
    } else {
        if (!channel->hasLimit()) return false;
        channel->setLimit(0);
        return true;
    }
}

Client* Executor::getClientFromChannel(Channel* channel, const std::string& nick) const {
    const std::vector<Client*>& members = channel->getMembers();
    for (size_t i = 0; i < members.size(); ++i) {
        if (members[i]->getNickname() == nick) return members[i];
    }
    return NULL;
}

bool Executor::applyModeO(Channel* channel, bool isAdding, const std::string& paramNick, Client* sender) {
    Client* target = getClientFromChannel(channel, paramNick);
    
    if (!target) {
        Reply::error(sender, ERR_USERNOTINCHANNEL, paramNick + " " + channel->getName(), "They aren't on that channel");
        return false;
    }

    if (isAdding) {
        if (channel->isOperator(target)) return false;
        channel->addOperator(target);
        return true;
    } else {
        if (!channel->isOperator(target)) return false;
        channel->removeOperator(target);
        return true;
    }
}