#pragma once

#include <memory>
#include "parser.h"
#include "scope.h"

struct BadSymbol
{
	Token token;
	bool exists;
	BadSymbol(Token t, bool e) : token(t), exists(e) {}
};

static inline std::ostream &operator<<(std::ostream &out, BadSymbol bs)
{
	out << bs.token << " is ";
	out << ((bs.exists) ? "already defined " : "undefined ");
	out << "in current scope";
	return out;
}

struct Analyzer
{
	StringTable stringTable;
	Parser parser;

	Analyzer(const char *file) : parser(file) {}

	void fail(Token token, bool exists)
	{
		throw BadSymbol(token, exists);
	}

	Toplevel toplevel()
	{
		auto toplevel = parser.toplevel();
		stringTable.build(toplevel);

		auto currentScope = std::make_shared<Scope>(Scope());
		toplevel.scope = currentScope;

		for (auto global : toplevel.globals) {
			global.match([&](Var &v) { var(v, currentScope); },
						 [&](Func &f) { func(f, currentScope); });
		}

		return toplevel;
	}

private:
	void define(Identifier &name, Ptr<Scope> currentScope)
	{
		auto n = stringTable.get(name);
		if (!currentScope->add(n, name)) {
			fail(name.token, true);
		}
	}

	void func(Func &f, Ptr<Scope> currentScope)
	{
		define(f.name, currentScope);

		auto funcScope = std::make_shared<Scope>(Scope());
		funcScope->parent = currentScope;
		f.body->scope = funcScope;

		for (auto param : f.parameters) {
			define(param, funcScope);
		}

		block(*f.body, funcScope);
	}

	void block(Block &b, Ptr<Scope> currentScope)
	{
		for (auto statement : b.statements) {
			statement->match([&](Var &v) { var(v, currentScope); },
				[&](If &i) { ifStatement(i, currentScope); },
				[&](While &w) { whileStatement(w, currentScope); },
				[&](For &f) { forStatement(f, currentScope); },
				[&](Return &r) { returnStatement(r, currentScope); },
				[&](Call &c) { call(c, currentScope); },
				[&](Operator &o) { oper(o, currentScope); });
		}
	}

	void condition(ConditionBase &cb, Ptr<Scope> currentScope)
	{
		expression(*cb.condition, currentScope);

		auto condScope = std::make_shared<Scope>(Scope());
		condScope->parent = currentScope;
		cb.body->scope = condScope;
		block(*cb.body, condScope);
	}

	void ifStatement(If &i, Ptr<Scope> currentScope)
	{
		condition(i, currentScope);

		if (i.elseBody != nullptr) {
			auto elseScope = std::make_shared<Scope>(Scope());
			elseScope->parent = currentScope;
			i.elseBody->scope = elseScope;
			block(*i.elseBody, elseScope);
		}
	}

	void whileStatement(While &w, Ptr<Scope> currentScope)
	{
		condition(w, currentScope);
	}

	void forStatement(For &f, Ptr<Scope> currentScope)
	{
		auto forScope = std::make_shared<Scope>(Scope());
		forScope->parent = currentScope;
		f.body->scope = forScope;

		expression(*f.from, currentScope);
		expression(*f.to, currentScope);
		define(f.variable, forScope);

		block(*f.body, forScope);
	}

	void returnStatement(Return &r, Ptr<Scope> currentScope)
	{
		expression(*r.returnValue, currentScope);
	}

	void var(Var &v, Ptr<Scope> currentScope)
	{
		define(v.name, currentScope);
		if (v.value != nullptr) {
			expression(*v.value, currentScope);
		}
	}

	void expression(Expression &exp, Ptr<Scope> currentScope)
	{
		exp.match([&](Operator &o) { oper(o, currentScope); },
				  [&](Call &c) { call(c, currentScope); },
				  [&](Atom &a) { atom(a, currentScope); });
	}

	void oper(Operator &op, Ptr<Scope> currentScope)
	{
		for (auto operand : op.operands) {
			expression(*operand, currentScope);
		}
	}

	void identifier(Identifier &name, Ptr<Scope> currentScope)
	{
		if (!stringTable.exists(name)) {
			fail(name.token, false);
		}
		auto id = stringTable.get(name);
		if (!currentScope->exists(id)) {
			fail(name.token, false);
		}
	}

	void call(Call &call, Ptr<Scope> currentScope)
	{
		identifier(call.function, currentScope);

		for (auto operand : call.operands) {
			expression(*operand, currentScope);
		}
	}

	void atom(Atom &atom, Ptr<Scope> currentScope)
	{
		atom.match([&](Identifier &i) { identifier(i, currentScope); });
	}
};
