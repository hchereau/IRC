// hardcoded by gpt for server test

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <sstream>

struct ServerConfig
{
	int         port; // n to verify which program(server) to connect
	// IP is like the adress of the building and port is to say which room cuz several server possible
	std::string password;
	std::string serverName;
	int         backlog;
	int         maxClients;
	bool        debugLog;



	ServerConfig()
		: port(6667),
		  password("test"),
		  serverName("irc.local"),
		  backlog(10),
		  maxClients(100),
		  debugLog(true)
	{
	}
	std::string toDebug()
	{
		std::ostringstream oss;

	oss << "ServerConfig{"
		<< "port=" << port
		<< ", password=" << password
		<< ", serverName=" << serverName
		<< ", backlog=" << backlog
		<< ", maxClients=" << maxClients
		<< ", debugLog=" << (debugLog ? "true" : "false")
		<< "}";

		return oss.str();
	}
};

#endif 

// hardcoded by gpt for server test