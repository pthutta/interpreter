#include <iostream>
#include <string>
#include "analyzer.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Please specify input file!" << std::endl;
		return 1;
	}

	Analyzer a(argv[1]);
	try {
		auto program = a.toplevel();
		std::cerr << program << std::endl;
	}
	catch (BadParse bp)
	{
		std::cerr << bp << std::endl;
	}
	catch (BadSymbol bs)
	{
		std::cerr << bs << std::endl;
	}

	return 0;
}