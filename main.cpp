#include <iostream>
#include <string>
#include "lexer.h"
#include "parser.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Please specify input file!" << std::endl;
		return 1;
	}

	Parser p(argv[1]);
	try {
		auto program = p.toplevel();
		std::cerr << program << std::endl;
	}
	catch (BadParse bp)
	{
		std::cerr << bp << std::endl;
	}

	return 0;
}