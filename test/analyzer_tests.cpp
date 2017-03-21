#include "catch.hpp"
#include "analyzer.h"

TEST_CASE("Scopes") {
	Toplevel tl;

	Analyzer a1("files/Error51_scopes.txt");
	REQUIRE_THROWS_AS(tl = a1.toplevel(), BadSymbol);

	Analyzer a2("files/Error52_scopes.txt");
	REQUIRE_THROWS_AS(tl = a2.toplevel(), BadSymbol);

	Analyzer a3("files/Error53_scopes.txt");
	REQUIRE_THROWS_AS(tl = a3.toplevel(), BadSymbol);

	Analyzer a4("files/Error54_scopes.txt");
	REQUIRE_THROWS_AS(tl = a4.toplevel(), BadSymbol);

	Analyzer a5("files/Error55_scopes.txt");
	REQUIRE_THROWS_AS(tl = a5.toplevel(), BadSymbol);
}

TEST_CASE("Examples2") {
	Toplevel tl;

	Analyzer a1("files/Euclid.txt");
	REQUIRE_NOTHROW(tl = a1.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a2("files/Factorial.txt");
	REQUIRE_NOTHROW(tl = a2.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a3("files/Fibonacci.txt");
	REQUIRE_NOTHROW(tl = a3.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a4("files/Perfect.txt");
	REQUIRE_NOTHROW(tl = a4.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a5("files/Pointless.txt");
	REQUIRE_NOTHROW(tl = a5.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a6("files/Pointless2.txt");
	REQUIRE_NOTHROW(tl = a6.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Analyzer a7("files/Prime.txt");
	REQUIRE_NOTHROW(tl = a7.toplevel());
	REQUIRE(tl.globals.size() > 0);
}