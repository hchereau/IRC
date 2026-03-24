// hardcoded by gpt for server test

#include "HardcodedConfig.hpp"

namespace HardcodedConfig
{
	ServerConfig makeDefault()
	{
		ServerConfig config;

		config.port = 6667;
		config.password = "test";
		config.serverName = "irc.local";
		config.backlog = 10;
		config.maxClients = 100;
		config.debugLog = true;

		return config;
	}
}

// hardcoded by gpt for server test
