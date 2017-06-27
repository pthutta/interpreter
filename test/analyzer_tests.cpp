#include "catch.hpp"
#include "analyzer.h"
#include "cwd.h"

TEST_CASE("Scopes") {
	Toplevel tl;

	Analyzer a1(cwd + std::string("files/Error51_scopes.txt"));
	REQUIRE_THROWS_AS(tl = a1.toplevel(), BadSymbol);

	Analyzer a2(cwd + std::string("files/Error52_scopes.txt"));
	REQUIRE_THROWS_AS(tl = a2.toplevel(), BadSymbol);

	Analyzer a3(cwd + std::string("files/Error53_scopes.txt"));
	REQUIRE_THROWS_AS(tl = a3.toplevel(), BadSymbol);

	Analyzer a4(cwd + std::string("files/Error54_scopes.txt"));
	REQUIRE_THROWS_AS(tl = a4.toplevel(), BadSymbol);

	Analyzer a5(cwd + std::string("files/Error55_scopes.txt"));
	REQUIRE_THROWS_AS(tl = a5.toplevel(), BadSymbol);
}

TEST_CASE("Examples2") {
	Toplevel tl;

	Analyzer a1(cwd + std::string("files/Euclid.txt"));
	REQUIRE_NOTHROW(tl = a1.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a2(cwd + std::string("files/Factorial.txt"));
	REQUIRE_NOTHROW(tl = a2.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a3(cwd + std::string("files/Fibonacci.txt"));
	REQUIRE_NOTHROW(tl = a3.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a4(cwd + std::string("files/Perfect.txt"));
	REQUIRE_NOTHROW(tl = a4.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a5(cwd + std::string("files/Pointless.txt"));
	REQUIRE_NOTHROW(tl = a5.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a6(cwd + std::string("files/Pointless2.txt"));
	REQUIRE_NOTHROW(tl = a6.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a7(cwd + std::string("files/Prime.txt"));
	REQUIRE_NOTHROW(tl = a7.toplevel());
	REQUIRE(tl.globals.size() > 0);
}