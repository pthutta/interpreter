#include <iostream>
#include <string>
#include "analyzer.h"
#include "evaluator.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Please specify input file!" << std::endl;
		return 1;
	}

	Evaluator e(argv[1]);
	try {
		e.evalAndPrint();
		//std::cerr << program << std::endl;
	}
	catch (BadParse bp)
	{
		std::cerr << bp << std::endl;
	}
	catch (BadSymbol bs)
	{
		std::cerr << bs << std::endl;
	}
	catch (RuntimeError re)
	{
		std::cerr << re << std::endl;
	}

	return 0;
}