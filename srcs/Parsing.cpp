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
	size_t end = line.size() - 1;
	size_t max = 510;

	if (!line.size())
		throw	std::invalid_argument("Line couldn't be read");
	std::string	cpy = line;

	if (cpy.size() > 510) // enlever les \r\n avant ou les inclure après le erase ?
		cpy.erase(max, end - max);
	if (!cpy.empty() && cpy[0] == ':')
		msg.prefix = extractPrefix(cpy);
	if (!cpy.empty())
		msg.cmd = extractCmd(cpy);
	if (!cpy.empty() && cpy.find(':') != std::string::npos)
		msg.trailing = extractTrailing(cpy);
	if (!cpy.empty())
		msg.params = extractParams(cpy);
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
	size_t start = 0;

	while (start < line.size() && line[start] == ' ')
		start++;

	size_t spacePos	= line.find(' ', start);

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
	for (size_t i = 0; i < cmd.size(); i++)
		cmd[i] = std::toupper(cmd[i]); 
	return (cmd);
}

std::string Parsing::extractTrailing(std::string& line)
{
	std::string		trailing;
	size_t end = line.size();
	size_t start = line.find(':');

	//std::cout << "line is : " << line << std::endl; // debug
	//std::cout << "end is : " << end << std::endl; // debug
	
	while (end > 0 && line[end - 1] == ' ')
		end--;
	trailing = line.substr(start + 1, end - (start + 1));

	//std::cout << "end is : " << end << std::endl; // debug
	//std::cout << "trailing is : " << trailing << std::endl;

	line.erase(start, end - start);

	//std::cout << "line is : " << line << std::endl; // debug
	return (trailing);
}

std::vector<std::string> Parsing::extractParams(std::string& line)
{
	std::vector<std::string>	params;
	size_t	i = 0;

	while (i < line.size())
	{
		while (i < line.size() && line[i] == ' ')
			i++;
		if (i >= line.size())
			break;

		size_t start = i;

		while (i < line.size() && line[i] != ' ')
			i++;
		std::string param = line.substr(start, i - start);
		params.push_back(param);
	}
	line.clear();

	return (params);
}
