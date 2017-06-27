#include "catch.hpp"
#include "evaluator.h"
#include "cwd.h"

TEST_CASE("Euclid") {
	Evaluator e(cwd + std::string("files/Euclid.txt"));
	std::vector<Value> correct{4, 21, 6, 12, 1, 1};
	std::vector<Value> values;

	REQUIRE_NOTHROW(values = e.eval());
	REQUIRE(values.size() == correct.size());
	REQUIRE(values == correct);
}

TEST_CASE("Factorial") {
	Evaluator e(cwd + std::string("files/Factorial.txt"));
	std::vector<Value> correct{1, 1, 2, 6, 24, 120, 720, 1, 1, 2, 6, 24, 120, 720};
	std::vector<Value> values;

	REQUIRE_NOTHROW(values = e.eval());
	REQUIRE(values.size() == correct.size());
	REQUIRE(values == correct);
}

TEST_CASE("Fibonacci") {
	Evaluator e(cwd + std::string("files/Fibonacci.txt"));
	std::vector<Value> correct{0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
	std::vector<Value> values;

	REQUIRE_NOTHROW(values = e.eval());
	REQUIRE(values.size() == correct.size());
	REQUIRE(values == correct);
}

TEST_CASE("Perfect") {
	Evaluator e(cwd + std::string("files/Perfect.txt"));
	std::vector<Value> correct{0, 0, 0,	1, 0, 1, 1,	0, 1, 0};
	std::vector<Value> values;

	REQUIRE_NOTHROW(values = e.eval());
	REQUIRE(values.size() == correct.size());
	REQUIRE(values == correct);
}

TEST_CASE("Prime") {
	Evaluator e(cwd + std::string("files/Prime.txt"));
	std::vector<Value> correct{0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0};
	std::vector<Value> values;

	REQUIRE_NOTHROW(values = e.eval());
	REQUIRE(values.size() == correct.size());
	REQUIRE(values == correct);
}