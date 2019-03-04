#include "Lexer.h"
#include <iostream>
#include <string>


int main(char **argc, int argv)
{
    std::string output;
	std::vector<std::string> tokens = { "toto", "[hHa]allo", "[0-9]+", "[ \n]+" };
	Lexer lexer(tokens);
	output.clear();
	lexer.print(output);
	std::cout << output;
	std::string input = "aallo toto \n 12432    ";
	bool isEnd = false;
	unsigned int offset = 0;
	do {
		auto result = lexer.match(input, offset);
		if (result.isEmpty())
		{
			isEnd = true;
		}
		else
		{
			auto &tokenMatch = result.getResult();
			offset += (unsigned int)tokenMatch.length();
			std::cout << "No. " << result.getIndex() << " -> " << tokenMatch << "\n";
		}
	} while (isEnd == false && offset < input.length());
    return 0;
}