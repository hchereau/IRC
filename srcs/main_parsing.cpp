#include "Parsing.hpp"
#include "Executor.hpp"
#include "Client.hpp"

int main(int argc, char **argv)
{
	Executor executor(NULL);
	Parsing parser;
	Client client(1, "localhost");

	if (argc != 2)
	{
		std::cout << "wrong usage, add arguments" << std::endl;
		return (0);
	}
	
	for (int i = 1; i < argc; i++)
		client.appendToReadBuffer(std::string(argv[i]) + "\r\n");

	std::string line;

	while (!(line = client.extractMessage()).empty())
	{
	    Message msg = parser.parseLine(line);

		if (msg.prefix != "")
				std::cout << "Prefix: " << msg.prefix << std::endl;	
		if (!msg.cmd.empty())
				std::cout << "CMD: " << msg.cmd << std::endl;
		if (!msg.params.empty())
		{
			for (size_t i = 0; i < msg.params.size(); ++i)
				std::cout << "Param number " << i << " is : "  << msg.params[i] << std::endl;
		}
		if (msg.trailing != "")
			std::cout << "Trailing: " << msg.trailing << std::endl;

	    executor.dispatchMessage(&client, msg); // mettre (void)_server; à dispatchMessage pour tester si ca compile // needs to test w Server
	}
}

// TESTS 

//"NICK toto");
//"     USER toto 0 #channel1 :Toto User");
//":prefix JOIN #42");
//"PRIVMSG #42 :hello world");
//"QUIT :bye");
//"QUIT :     you should    keep the    spaces       ");
//"quit :bye");
	// cas limites
//""
//"         "
//"PRIVMSG #chan :hello :world"
//"PRIVMSG #chan :"
//":nick!user@host PRIVMSG #chan :hello"