#include "Parsing.hpp"
#include "Executor.hpp"
#include "Client.hpp"

int main(int argc, char **argv)
{
	Executor executor(NULL);
	Parsing parser;

	Client client(1, "localhost");

	std::vector<std::string> tests;
	for (int i = 1; i < argc; i++)
		tests.push_back(argv[i]);
	//if (argc != 2)
	//{
	//	std::cout << "wrong usage, add arguments" << std::endl;
	//	return (0);
	//}
	// tests de plusieurs cas
	tests.push_back("NICK toto");
	tests.push_back("     USER toto 0 #channel1 :Toto User");
	tests.push_back(":prefix JOIN #42");
	tests.push_back("PRIVMSG #42 :hello world");
	tests.push_back("QUIT :bye");
	tests.push_back("QUIT :     you should    keep the    spaces       ");
	tests.push_back("quit :bye");
	// cas limites
	tests.push_back("");
	tests.push_back("         ");
	tests.push_back("PRIVMSG #chan :hello :world");
	tests.push_back("PRIVMSG #chan :");
	tests.push_back(":nick!user@host PRIVMSG #chan :hello");
	

	for (size_t i = 0; i < tests.size(); i++)
	{
		std::cout << std::endl;
		std::cout << "Input: " << tests[i] << std::endl;

		Message msg = parser.parseLine(tests[i]);

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

	return 0;
}

/*
std::string line = client.getMessageBuffer();

std::string command;
std::vector<std::string> params;

// parsing ici

if (command == "JOIN")
	joinCommand(...) */