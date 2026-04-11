#include "Executor.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include "CommandValidator.hpp"

void	Executor::execTopic(Client* client, const Message& msg)
{
	if (msg.params.empty())
	{
		Reply::error(client, ERR_NEEDMOREPARAMS, "TOPIC", "Not enough parameters");
		return;
	}

	std::string	channelName = msg.params[0];
	Channel* channel = _server->getChannelByName(channelName);

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

	if (msg.params.size() == 1 && msg.trailing.empty())
	{
		std::string	topic = channel->getTopic();
		if (topic.empty())
			Reply::custom(client, RPL_NOTOPIC, client->getNickname() + " " + channelName + " :No topic is set");
		else
			Reply::custom(client, RPL_TOPIC, client->getNickname() + " " + channelName + " :" + topic);
		return ;
	}

	if (channel->isTopicRestricted() && !channel->isOperator(client))
	{
		Reply::error(client, ERR_CHANOPRIVSNEEDED, "TOPIC", channelName);
		return;
	}

	std::string newTopic;
	if (msg.params.size() > 1)
		newTopic = msg.params[1];
	else
		newTopic = msg.trailing;

	channel->setTopic(newTopic);

	std::string prefix = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname();
	std::string broadcast = prefix + " TOPIC " + channelName + " :" + newTopic + "\r\n";
	channel->broadcastMessage(broadcast, NULL); 
}