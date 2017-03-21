#include "catch.hpp"
#include "parser.h"

TEST_CASE("Examples") {
	Toplevel tl;

	Parser p1("files/Euclid.txt");
	REQUIRE_NOTHROW(tl = p1.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Parser p2("files/Factorial.txt");
	REQUIRE_NOTHROW(tl = p2.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Parser p3("files/Fibonacci.txt");
	REQUIRE_NOTHROW(tl = p3.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Parser p4("files/Perfect.txt");
	REQUIRE_NOTHROW(tl = p4.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Parser p5("files/Pointless.txt");
	REQUIRE_NOTHROW(tl = p5.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Parser p6("files/Pointless2.txt");
	REQUIRE_NOTHROW(tl = p6.toplevel());
	REQUIRE(tl.globals.size() > 0);

	Parser p7("files/Prime.txt");
	REQUIRE_NOTHROW(tl = p7.toplevel());
	REQUIRE(tl.globals.size() > 0);
}

TEST_CASE("For BadParse") {
	For f;

	Parser p1("files/Error04_for.txt");
	p1.shift();
	REQUIRE_THROWS_AS(f = p1.forStatement(), BadParse);

	Parser p2("files/Error05_for.txt");
	p2.shift();
	REQUIRE_THROWS_AS(f = p2.forStatement(), BadParse);

	Parser p3("files/Error06_for.txt");
	p3.shift();
	REQUIRE_THROWS_AS(f = p3.forStatement(), BadParse);

	Parser p4("files/Error07_for.txt");
	p4.shift();
	REQUIRE_THROWS_AS(f = p4.forStatement(), BadParse);

	Parser p5("files/Error08_for.txt");
	p5.shift();
	REQUIRE_THROWS_AS(f = p5.forStatement(), BadParse);

	Parser p6("files/Error09_for.txt");
	p6.shift();
	REQUIRE_THROWS_AS(f = p6.forStatement(), BadParse);

	Parser p7("files/Error10_for.txt");
	p7.shift();
	REQUIRE_THROWS_AS(f = p7.forStatement(), BadParse);

	Parser p8("files/Error11_for.txt");
	p8.shift();
	REQUIRE_THROWS_AS(f = p8.forStatement(), BadParse);

	Parser p9("files/Error12_for.txt");
	p9.shift();
	REQUIRE_THROWS_AS(f = p9.forStatement(), BadParse);
}

TEST_CASE("While BadParse") {
	While w;

	Parser p1("files/Error13_while.txt");
	p1.shift();
	REQUIRE_THROWS_AS(w = p1.whileStatement(), BadParse);

	Parser p2("files/Error14_while.txt");
	p2.shift();
	REQUIRE_THROWS_AS(w = p2.whileStatement(), BadParse);

	Parser p3("files/Error15_while.txt");
	p3.shift();
	REQUIRE_THROWS_AS(w = p3.whileStatement(), BadParse);

	Parser p4("files/Error16_while.txt");
	p4.shift();
	REQUIRE_THROWS_AS(w = p4.whileStatement(), BadParse);

	Parser p5("files/Error17_while.txt");
	p5.shift();
	REQUIRE_THROWS_AS(w = p5.whileStatement(), BadParse);
}

TEST_CASE("If BadParse") {
	If i;

	Parser p1("files/Error18_if.txt");
	p1.shift();
	REQUIRE_THROWS_AS(i = p1.ifStatement(), BadParse);

	Parser p2("files/Error19_if.txt");
	p2.shift();
	REQUIRE_THROWS_AS(i = p2.ifStatement(), BadParse);

	Parser p3("files/Error20_if.txt");
	p3.shift();
	REQUIRE_THROWS_AS(i = p3.ifStatement(), BadParse);

	Parser p4("files/Error21_if.txt");
	p4.shift();
	REQUIRE_THROWS_AS(i = p4.ifStatement(), BadParse);

	Parser p5("files/Error22_if.txt");
	p5.shift();
	REQUIRE_THROWS_AS(i = p5.ifStatement(), BadParse);

	Parser p6("files/Error23_if.txt");
	p6.shift();
	REQUIRE_THROWS_AS(i = p6.ifStatement(), BadParse);

	Parser p7("files/Error24_if.txt");
	p7.shift();
	REQUIRE_THROWS_AS(i = p7.ifStatement(), BadParse);
}

TEST_CASE("Expression BadParse") {
	Ptr<Expression> e;

	Parser p1("files/Error31_expression.txt");
	p1.shift();
	REQUIRE_THROWS_AS(e = p1.expression(), BadParse);

	Parser p2("files/Error32_expression.txt");
	p2.shift();
	REQUIRE_THROWS_AS(e = p2.expression(), BadParse);

	Parser p3("files/Error33_expression.txt");
	p3.shift();
	REQUIRE_THROWS_AS(e = p3.expression(), BadParse);

	Parser p4("files/Error34_expression.txt");
	p4.shift();
	REQUIRE_THROWS_AS(e = p4.expression(), BadParse);

	Parser p5("files/Error35_expression.txt");
	p5.shift();
	REQUIRE_THROWS_AS(e = p5.expression(), BadParse);

	Parser p6("files/Error36_expression.txt");
	p6.shift();
	REQUIRE_THROWS_AS(e = p6.expression(), BadParse);

	Parser p7("files/Error37_expression.txt");
	p7.shift();
	REQUIRE_THROWS_AS(e = p7.expression(), BadParse);

	Parser p8("files/Error38_expression.txt");
	p8.shift();
	REQUIRE_THROWS_AS(e = p8.expression(), BadParse);

	Parser p9("files/Error39_expression.txt");
	p9.shift();
	REQUIRE_THROWS_AS(e = p9.expression(), BadParse);

	Parser p10("files/Error40_expression.txt");
	p10.shift();
	REQUIRE_THROWS_AS(e = p10.expression(), BadParse);

	Parser p11("files/Error41_expression.txt");
	p11.shift();
	REQUIRE_THROWS_AS(e = p11.expression(), BadParse);

	Parser p12("files/Error42_expression.txt");
	p12.shift();
	REQUIRE_THROWS_AS(e = p12.expression(), BadParse);

	Parser p13("files/Error43_expression.txt");
	p13.shift();
	REQUIRE_THROWS_AS(e = p13.expression(), BadParse);

	Parser p14("files/Error44_expression.txt");
	p14.shift();
	REQUIRE_THROWS_AS(e = p14.expression(), BadParse);

	Parser p15("files/Error45_expression.txt");
	p15.shift();
	REQUIRE_THROWS_AS(e = p15.expression(), BadParse);
}

TEST_CASE("Block BadParse") {
	Ptr<Block> b;

	Parser p1("files/Error46_block.txt");
	p1.shift();
	REQUIRE_THROWS_AS(b = p1.block(), BadParse);

	Parser p2("files/Error47_block.txt");
	p2.shift();
	REQUIRE_THROWS_AS(b = p2.block(), BadParse);

	Parser p3("files/Error48_block.txt");
	p3.shift();
	REQUIRE_THROWS_AS(b = p3.block(), BadParse);

	Parser p4("files/Error49_block.txt");
	p4.shift();
	REQUIRE_THROWS_AS(b = p4.block(), BadParse);

	Parser p5("files/Error50_block.txt");
	p5.shift();
	REQUIRE_THROWS_AS(b = p5.block(), BadParse);
}

TEST_CASE("Func BadParse") {
	Func f;

	Parser p1("files/Error25_func.txt");
	p1.shift();
	REQUIRE_THROWS_AS(f = p1.func(), BadParse);

	Parser p2("files/Error26_func.txt");
	p2.shift();
	REQUIRE_THROWS_AS(f = p2.func(), BadParse);

	Parser p3("files/Error27_func.txt");
	p3.shift();
	REQUIRE_THROWS_AS(f = p3.func(), BadParse);

	Parser p4("files/Error28_func.txt");
	p4.shift();
	REQUIRE_THROWS_AS(f = p4.func(), BadParse);

	Parser p5("files/Error29_func.txt");
	p5.shift();
	REQUIRE_THROWS_AS(f = p5.func(), BadParse);

	Parser p6("files/Error30_func.txt");
	p6.shift();
	REQUIRE_THROWS_AS(f = p6.func(), BadParse);
}

TEST_CASE("Others") {
	Toplevel tl;

	Parser p1("files/Error01.txt");
	REQUIRE_THROWS_AS(tl = p1.toplevel(), BadParse);

	Parser p2("files/Error02.txt");
	REQUIRE_THROWS_AS(tl = p2.toplevel(), BadParse);

	Parser p3("files/Error03.txt");
	REQUIRE_THROWS_AS(tl = p3.toplevel(), BadParse);
}
