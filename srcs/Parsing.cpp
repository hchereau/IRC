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
	size_t spacePos	= line.find(' ');

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
		return "";
	cmd = line.substr(1, spacePos - 1);
	line.erase(0, spacePos + 1);
	for (size_t i = 0; i < cmd.size(); i++)
		cmd[i] = std::toupper(cmd[i]);
	return (cmd);
}

/*

std::string Parsing::extractTrailing(std::string& line)
{
	if ()
}

1. copy line → workingLine

2. prefix = extractPrefix()

3. trailing = extractTrailing()   ⚠️ IMPORTANT

4. cmd = extractCmd()

5. params = extractParams()

Message Parser::parseLine(string line) {
  // 1. if line[0]==':' → extract prefix (jusqu'espace)
  // 2. extract cmd (jusqu'espace) → toupper()
  // 3. boucle: params jusqu'à ':' → trailing
  // 4. check len<512
}

*/