#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

void	Executor::execTopic(Client* client, const Message& msg)
{
	if (msg.params.empty() && msg.trailing.empty())
	{
		Reply::error(client, ERR_NEEDMOREPARAMS, "TOPIC", "Not enough parameters");
		return;
	}
	std::string	channelName;
	 if (!msg.params.empty())
		channelName = msg.params[0];
	else if (!msg.trailing.empty())
		channelName = msg.trailing;

	Channel*	channel = _server->getChannelByName(channelName);
	// verifier si channel existe
	if (!channel)
	{
		Reply::error(client, ERR_NOSUCHCHANNEL, "TOPIC", channelName);
		return;
	}
	if (!channel->isMember(client))
	{
		Reply::error(client, ERR_NOTONCHANNEL, "TOPIC", channelName);
		return;
	}
	// afficher channel
	if (msg.trailing.empty())
	{
		std::string	topic = channel->getTopic();
		if (topic.empty())
			Reply::custom(client, RPL_NOTOPIC, client->getNickname() + " " + channelName + " :No topic is set");
		else
			Reply::custom(client, RPL_TOPIC, client->getNickname() + " " + channelName + " :" + topic);
		return ;
	}
	// verifier permissions
	if (channel->isTopicRestricted() && !channel->isOperator(client))
	{
		Reply::error(client, ERR_CHANOPRIVSNEEDED, "TOPIC", channelName);
		return;
	}
	// modifier
	std::string newTopic = msg.trailing;
	channel->setTopic(newTopic);
	// notifier
	std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
	std::string broadcast = prefix + " TOPIC " + channelName + " :" + newTopic + "\r\n";
	channel->broadcastMessage(broadcast, client);
}
