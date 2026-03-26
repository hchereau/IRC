#include "Parsing.hpp"

/*================ CONSTRUCTOR ================*/

Parsing::Parsing()
{
}


Parsing::Parsing(const Parsing &)
{
}

/*================== OPERATOR =================*/

Parsing &Parsing::operator=(const Parsing &)
{
	return (*this);
}

/*================= DESTRUCTOR ================*/

Parsing::~Parsing()
{
}

/*================== METHODS ==================*/

Message	Parsing::parseLine(const std::string& line)
{
	Message		msg;

	if (!line.size() || line.size() > 510)
	{
		std::cerr << "Message is too long" << std::endl;
		return(msg);
	}
	std::string	cpy = line;

	if (!cpy.empty())
	{
		std::cerr << "Pb with cpy" << std::endl;
		return(msg);	
	}
	if (cpy[0] == ':')
		msg.prefix = extractPrefix(cpy);
	if (!cpy.empty())
		msg.cmd = extractCmd(cpy);
	//msg.trailing = extractTrailing(cpy);
	//cpy.extractParams();
	return (msg);
}

std::string Parsing::extractPrefix(std::string& line)
{
	std::string		prefix;
	size_t start = 0;

	while (start < line.size() && line[start] == ' ')
		start++;

	size_t spacePos	= line.find(' ', start);

	if (spacePos == std::string::npos)
		return "";
	prefix = line.substr(1, spacePos - 1);
	line.erase(0, spacePos + 1);
	return (prefix);
}

std::string Parsing::extractCmd(std::string& line)
{
	std::string	cmd;
	size_t spacePos	= line.find(' ');

	if (spacePos == std::string::npos)
		cmd = line.substr(0, std::string::npos - 1);
	cmd = line.substr(0, spacePos - 1);
	line.erase(0, spacePos + 1);
	for (size_t i = 0; i < cmd.size(); i++)
		cmd[i] = std::toupper(cmd[i]);
	return (cmd);
}

std::string Parsing::extractCmd(std::string& line)
{
    std::string cmd;

    // Ignorer les espaces au début
    size_t start = 0;
    while (start < line.size() && line[start] == ' ')
        start++;

    // Trouver fin de commande
    size_t spacePos = line.find(' ', start);

    if (spacePos == std::string::npos)
    {
        cmd = line.substr(start);
        line.clear();
    }
    else
    {
        cmd = line.substr(start, spacePos - start);
        line.erase(0, spacePos + 1);
    }

    // Mettre en majuscule
    for (size_t i = 0; i < cmd.size(); i++)
        cmd[i] = std::toupper(cmd[i]);

    return cmd;
}


/*

*/