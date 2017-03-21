#pragma once
#include <memory>
#include <vector>
#include <brick-types>
#include "lexer.h"

using brick::types::Union;

template< typename T >
using Ptr = std::shared_ptr< T >;

struct Expression;

struct Identifier
{
	Token token;

	Identifier() : token(Token::Eof, "", 0) {}
};

using Literal = Union<std::string, int>;

using Atom = Union<Identifier, Literal>;

struct OperBase
{
	std::vector<Ptr<Expression>> operands;
};

struct Call : OperBase
{
	Identifier function;
};

struct Operator : OperBase
{
	Token token = {Token::Eof, "", 0};
};

using ExprBase = Union<Operator, Call, Atom>;
struct Expression : ExprBase
{
	explicit Expression(ExprBase b) : ExprBase(b) {}
};

struct Block;
struct Scope;

struct ConditionBase
{
	Ptr<Expression> condition;
	Ptr<Block> body;
};

struct If : ConditionBase
{
	Ptr<Block> elseBody;
};
struct While : ConditionBase {};

struct For
{
	Identifier variable;
	Ptr<Expression> from;
	Ptr<Expression> to;
	Ptr<Block> body;
	bool downto;
};

struct Return
{
	Ptr<Expression> returnValue;
};

struct Var
{
	Identifier name;
	Ptr<Expression> value;
};

using Statement = Union<Var, If, While, For, Return, Call, Operator>;
struct Block
{
	std::vector<Ptr<Statement>> statements;
	Ptr<Scope> scope;
};

struct Func 
{
	Identifier name;
	std::vector<Identifier> parameters;
	Ptr<Block> body;
};

using Global = Union<Func, Var>;

struct Toplevel
{
	std::vector<Global> globals;
	Ptr<Scope> scope;
};

inline std::ostream &operator<<(std::ostream &out, Expression ex);

inline std::ostream &operator<<(std::ostream &out, Literal l) {
	l.match([&](std::string s) { out << "\"" << s << "\""; },
		[&](int i) { out << i; });
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Identifier i) {
	out << i.token.text;
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Atom a) {
	a.match([&](Identifier i) { out << i; },
			[&](Literal l) { out << l; });
	return out;
}

inline void operatorCheck(std::ostream &out, Expression ex) {
	ex.match([&](Operator o) { out << "("; });
	out << ex;
	ex.match([&](Operator o) { out << ")"; });
}

inline std::ostream &operator<<(std::ostream &out, std::vector<Ptr<Expression>> opers) {
	for (int i = 0; i < opers.size(); i++) {
		operatorCheck(out, *opers[i]);

		if (i < opers.size() - 1) {
			out << " ";
		}
	}
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Call c) {
	out << c.function << "(" << c.operands << ")";
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Operator o) {
	out << o.token.text << " " << o.operands;
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Expression ex) {
	ex.match([&](Operator o) { out << o; },
			 [&](Call c) { out << c; },
			 [&](Atom a) { out << a; });
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Var v) {
	out << "var " << v.name;
	if (v.value != nullptr) {
		out << " ";
		operatorCheck(out, *v.value);
	}
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Return r) {
	out << "return " << *r.returnValue;
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Block b);

inline std::ostream &operator<<(std::ostream &out, ConditionBase c) {
	out << "(" << *c.condition << ") " << *c.body;
	return out;
}

inline std::ostream &operator<<(std::ostream &out, If i) {
	out << "if " << ConditionBase(i);
	if (i.elseBody != nullptr) {
		out << std::endl << "else " << *i.elseBody;
	}
	return out;
}

inline std::ostream &operator<<(std::ostream &out, While w) {
	out << "while " << ConditionBase(w);
	return out;
}

inline std::ostream &operator<<(std::ostream &out, For f) {
	out << "for (var " << f.variable << " from ";
	operatorCheck(out, *f.from);
	out << ((f.downto) ? " downto " : " to ");
	operatorCheck(out, *f.to);
	out << ") " << *f.body;
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Block b) {
	out << "(" << std::endl;
	for (auto s : b.statements) {
		out << "(";
		s->match([&](Var v) { out << v; },
				[&](If i) { out << i; },
				[&](While w) { out << w; },
				[&](For f) { out << f; }, 
				[&](Return r) { out << r; }, 
				[&](Call c) { out << c; }, 
				[&](Operator o) { out << o; });
		out << ")" << std::endl;
	}
	out << ")";
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Func f) {
	out << "func " << f.name << "(";
	for (int i = 0; i < f.parameters.size(); i++) {
		out << f.parameters[i];
		if (i < f.parameters.size()-1) {
			out << " ";
		}
	}
	out << ") " << *f.body;
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Global g) {
	g.match([&](Func f) { out << f; },
			[&](Var v) { out << v; });
	return out;
}

inline std::ostream &operator<<(std::ostream &out, Toplevel tl) {
	for (auto g : tl.globals) {
		out << g << std::endl;
	}
	return out;
}