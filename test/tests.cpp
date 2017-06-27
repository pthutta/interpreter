#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "cwd.h"

std::string cwd;

int main( int argc, char* argv[] )
{
	std::string path(argv[0]);
	cwd = path.substr(0, path.find_last_of("\\/")+1);

	int result = Catch::Session().run( argc, argv );

	return ( result < 0xff ? result : 0xff );
}