#pragma once
#include <memory>
#include <dlfcn.h>
#include <algorithm>
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
	Parser parser;
	std::size_t varCounter;
	std::size_t globalVars;

	Analyzer(const char *file) : parser(file), varCounter(0), globalVars(0) {}

	Analyzer(std::string file) : Analyzer(file.c_str()) {}

	void fail(Token token, bool exists)
	{
		throw BadSymbol(token, exists);
	}

	Toplevel toplevel()
	{
		auto toplevel = parser.toplevel();

		auto currentScope = std::make_shared<Scope>(Scope());
		toplevel.scope = currentScope;

		for (auto & global : toplevel.globals) {
			global.match([&](Var &v) { var(v, currentScope, true); },
						 [&](Func &f) { func(f, currentScope); },
						 [&](Call &c) { call(c, currentScope, false); });
		}
		toplevel.globalVars = globalVars;
		return toplevel;
	}

private:
	void define(Identifier &name, Ptr<Scope> currentScope, bool global)
	{
		auto n = parser.stringTable.get(name);
		if (isSysCall(name)) {
			fail(name.token, true);
		}
		if (!currentScope->add(n, name, (global) ? globalVars : varCounter, global)) {
			fail(name.token, true);
		}
	}

	bool isSysCall(Identifier id)
	{
		auto name = id.token.text;
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		return dlsym(NULL, name.c_str()) != NULL;
	}

	void func(Func &f, Ptr<Scope> currentScope)
	{
		define(f.name, currentScope, true);
		varCounter = 0;

		auto funcScope = std::make_shared<Scope>(Scope());
		funcScope->parent = currentScope;
		f.body->scope = funcScope;

		for (auto & param : f.parameters) {
			define(param, funcScope, false);
			++varCounter;
		}

		block(*f.body, funcScope, true);
		f.frameSize = varCounter+1;
	}

	void block(Block &b, Ptr<Scope> currentScope, bool function)
	{
		for (auto statement : b.statements) {
			bool tailContext = function && statement == b.statements.back();
			statement->match([&](Var &v) { var(v, currentScope, false); },
				[&](If &i) { ifStatement(i, currentScope); },
				[&](While &w) { whileStatement(w, currentScope); },
				[&](For &f) { forStatement(f, currentScope); },
				[&](Return &r) { returnStatement(r, currentScope); },
				[&](Call &c) { call(c, currentScope, tailContext); },
				[&](Operator &o) { oper(o, currentScope, tailContext); });
		}
	}

	void condition(ConditionBase &cb, Ptr<Scope> currentScope)
	{
		expression(*cb.condition, currentScope, false);

		auto condScope = std::make_shared<Scope>(Scope());
		condScope->parent = currentScope;
		cb.body->scope = condScope;
		block(*cb.body, condScope, false);
	}

	void ifStatement(If &i, Ptr<Scope> currentScope)
	{
		condition(i, currentScope);

		if (i.elseBody != nullptr) {
			auto elseScope = std::make_shared<Scope>(Scope());
			elseScope->parent = currentScope;
			i.elseBody->scope = elseScope;
			block(*i.elseBody, elseScope, false);
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

		expression(*f.from, currentScope, false);
		expression(*f.to, currentScope, false);
		define(f.variable, forScope, false);
		++varCounter;

		block(*f.body, forScope, false);
	}

	void returnStatement(Return &r, Ptr<Scope> currentScope)
	{
		expression(*r.returnValue, currentScope, true);
	}

	void var(Var &v, Ptr<Scope> currentScope, bool global)
	{
		define(v.name, currentScope, global);
		if (v.value != nullptr) {
			expression(*v.value, currentScope, false);
		}
		if (global) {
			globalVars++;
		}
		else {
			varCounter++;
		}
	}

	void expression(Expression &exp, Ptr<Scope> currentScope, bool tailContext)
	{
		exp.match([&](Operator &o) { oper(o, currentScope, tailContext); },
				  [&](Call &c) { call(c, currentScope, tailContext); },
				  [&](Atom &a) { atom(a, currentScope); });
	}

	void oper(Operator &op, Ptr<Scope> currentScope, bool tailContext)
	{
		for (auto operand : op.operands) {
			expression(*operand, currentScope, tailContext && operand == op.operands.back());
		}
	}

	void identifier(Identifier &name, Ptr<Scope> currentScope)
	{
		if (!parser.stringTable.exists(name)) {
			fail(name.token, false);
		}
		auto id = parser.stringTable.get(name);
		if (!currentScope->exists(id)) {
			fail(name.token, false);
		}
	}

	void call(Call &call, Ptr<Scope> currentScope, bool tailContext)
	{
		if (isSysCall(call.function)) {
			call.isSysCall = true;
		}
		else {
			identifier(call.function, currentScope);
		}
		call.isTail = tailContext;

		for (auto operand : call.operands) {
			expression(*operand, currentScope, tailContext && operand == call.operands.back());
		}
	}

	void atom(Atom &atom, Ptr<Scope> currentScope)
	{
		atom.match([&](Identifier &i) { identifier(i, currentScope); });
	}
};
