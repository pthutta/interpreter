#pragma once
#include <set>
#include <unordered_map>
#include <string>
#include "ast.h"

struct IdNumber
{
	int value;

	IdNumber(int value) : value(value) {}
};

struct StringTable
{
	std::set<std::string> stringTable;

	IdNumber get(Identifier id)
	{
		return IdNumber(std::distance(stringTable.begin(), stringTable.find(id.token.text)));
	}

	bool exists(Identifier id)
	{
		return stringTable.find(id.token.text) != stringTable.end();
	}

	void build(Toplevel tl) {
		for (auto g : tl.globals) {
			g.match([&](Func f) { func(f); },
					[&](Var v) { var(v); });
		}
	}

private:
	void identifier(Identifier id) {
		stringTable.insert(id.token.text);
	}

	void func(Func f) {
		identifier(f.name);
		for (auto i : f.parameters) {
			identifier(i);
		}
		block(*f.body);
	}

	void var(Var v) {
		identifier(v.name);
		if (v.value != nullptr) {
			expression(*v.value);
		}
	}

	void expression(Expression exp) {
		exp.match([&](Operator o) { oper(o); },
		        [&](Call c) { call(c); },
		        [&](Atom a) { atom(a); });
	}

	void oper(Operator op) {
		for (auto operand : op.operands) {
			expression(*operand);
		}
	}

	void call(Call call)
	{
		identifier(call.function);
		for (auto operand : call.operands) {
			expression(*operand);
		}
	}

	void atom(Atom atom)
	{
		atom.match([&](Identifier i) { identifier(i); });
	}

	void condition(ConditionBase cb) {
		expression(*cb.condition);
	}

	void ifStatement(If i) {
		condition(i);
		block(*i.body);
		if (i.elseBody != nullptr) {
			block(*i.elseBody);
		}
	}

	void whileStatement(While w) {
		condition(w);
	}

	void forStatement(For f) {
		identifier(f.variable);
		expression(*f.from);
		expression(*f.to);
		block(*f.body);
	}

	void returnStatement(Return r) {
		expression(*r.returnValue);
	}

	void block(Block b) {
		for (auto statement : b.statements) {
			statement->match([&](Var &v) { var(v); },
			                [&](If &i) { ifStatement(i); },
			                [&](While &w) { whileStatement(w); },
			                [&](For &f) { forStatement(f); },
			                [&](Return &r) { returnStatement(r); },
			                [&](Call &c) { call(c); },
			                [&](Operator &o) { oper(o); });
		}
	}
};

struct Symbol
{
	Identifier identifier;
	//type
	//offset

	Symbol(Identifier identifier) : identifier(identifier) {}
};

struct Scope
{
	std::unordered_map<int, Symbol> symbolTable;
	std::shared_ptr<Scope> parent;

	bool add(IdNumber n, Identifier id)
	{
		auto val = symbolTable.insert(std::make_pair(n.value, Symbol(id)));
		return val.second;
	}

	bool exists(IdNumber n)
	{
		bool found = symbolTable.find(n.value) != symbolTable.end();
		if (!found && parent != nullptr) {
			return parent->exists(n);
		}
		return found;
	}
};