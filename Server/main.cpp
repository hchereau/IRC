#include "Server.hpp"
#include "HardcodedConfig.hpp"

int main(int ac, char **av)
{

	ServerConfig config = HardcodedConfig::makeDefault();

	std::cout << config.toDebug() << std::endl;

	// gogo
	Server server(config.port);
	
	server.runServer();
	server.cleanDown();
	return 0;
}


// 소켓은 AF_INET SOCK_STREAM
// 포트는 config.port
// listen 큐는 config.backlog
// socket bind listen accept poll