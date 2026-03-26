#include "Parsing.hpp"

int main(int argc, char **argv)
{
	Parsing		p;

	if (argc != 2)
	{
		std::cout << "wrong usage, add arguments" << std::endl;
		return (0);
	}
	Message msg = p.parseLine(argv[2]);

	if (!msg.prefix.empty())
		std::cout << "Prefix: " << msg.prefix << std::endl;
	std::cout << "Cmd: '" << msg.cmd << std::endl;
	//if (!msg.params.empty())
	//{
	//	for (size_t i = 0; i < msg.params.size(); ++i)
	//		std::cout << "Param number " << i << "is : "  << msg.params[i] << std::endl;
	//}
    return (0);
}
