#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

// 채널 클래스에 있는 isMember 로 계속 loop 돌기.. 

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
        if (_server->getChannelByName(chanName) != NULL){
            Channel *channel = _server->getChannelByName(chanName);
            if (!(channel->isMember(client))){
                Reply::error(client, ERR_NOTONCHANNEL, chanName, "You're not on that channel");
                continue;
            }
        }
        // get out of channel(client);
        // "leave channel(s)" 여럿이면 and 넣어주기? 
    }
}

// params[1] 은 채널 명임 이 채널 명에서 #를 떼서 <channel> :You're not on that channel 해야함 어케함 ?
// 각각의 채널 요청에 대한 케이스들을 개별적으로 처리해야 

static void error(Client* client, int code, const std::string& command, const std::string& msg);

void Executor::execJoin(Client* client, const Message& msg) {
    if (!CommandValidator::hasMinParams(msg.params, 1)) {
        Reply::error(client, ERR_NEEDMOREPARAMS, "JOIN", "Not enough parameters");
        return;
    }

    std::string channelName = msg.params[0];

    if (!CommandValidator::isValidChannelName(channelName)) {
        Reply::error(client, ERR_NOSUCHCHANNEL, channelName, "No such channel");
        return;
    }

    // TODO: Implémenter la suite avec _server->getOrCreateChannel(channelName)
}

/*

4.2.2 Part message

      Command: PART
   Parameters: <channel>{,<channel>}

 The PART message causes the client sending the message to be removed
   from the list of active users for all given channels listed in the
   parameter string.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_NOTONCHANNEL

   Examples:

   PART #twilight_zone             ; leave channel "#twilight_zone"

   PART #oz-ops,&group5            ; leave both channels "&group5" and
                                   "#oz-ops".

*/