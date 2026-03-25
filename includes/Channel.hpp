#pragma once

#include <string>
#include <vector>
#include "Client.hpp" 

class Channel {
	public:
	//default
		Channel(const std::string& name);
		~Channel();

	//setters/getters
		const std::string& getName() const;
		const std::string& getTopic() const;
		void setTopic(const std::string& topic);

	//Client management
		void addClient(Client* client);
		void removeClient(Client* client);
		bool isMember(Client* client) const;


	//operators
		void addOperator(Client* client);
		void removeOperator(Client* client);
		bool isOperator(Client* client) const;

	//modes
		// Mode +i (Invite-only)
		bool isInviteOnly() const;
		void setInviteOnly(bool i);
		
		// Mode +t (Op topic)
		bool isTopicRestricted() const;
		void setTopicRestricted(bool t);
		
		// Mode +k (password)
		bool hasKey() const;
		const std::string& getKey() const;
		void setKey(const std::string& key);
		
		// Mode +l (users limit)
		bool hasLimit() const;
		unsigned int getLimit() const;
		void setLimit(unsigned int limit); // 0 means no limite

		void broadcastMessage(const std::string& message, Client* sender);

	private:
		Channel();

		std::string _name;
		std::string _topic;

		std::vector<Client*> _members;
		std::vector<Client*> _operators;

		bool 			_inviteOnly;      // +i
		bool 			_topicRestricted; // +t
		std::string 	_key;             // +k
		unsigned int 	_limit;           // +l
};