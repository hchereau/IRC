#ifndef PARSING_HPP
# define PARSING_HPP

# include <vector>
# include <iostream>
# include <cstring>
# include <exception>
# include <cctype>

struct Message {
	std::string prefix;
	std::string cmd;
	std::vector<std::string> params;
	std::string trailing;
};

class Parsing
{
private:
	std::string 				extractPrefix(std::string& line);
	std::string 				extractCmd(std::string& line);
	std::vector<std::string>	extractParams(std::string& line);
	std::string 				extractTrailing(std::string& line);
	
public:
	Parsing();
	Parsing(Parsing const &);
	~Parsing();
	Parsing	&operator=(Parsing const &);

	Message parseLine(const std::string& line);

};

#endif