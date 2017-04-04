#pragma once
#include <set>
#include <unordered_map>
#include <string>
#include <brick-types>
#include "ast.h"

struct IdNumber
{
	int value;

	IdNumber(int value) : value(value) {}
};

struct StringTable {
	std::set<std::string> stringTable;

	IdNumber get(Identifier id)
	{
		return IdNumber(std::distance(stringTable.begin(), stringTable.find(id.token.text)));
	}

	bool exists(Identifier id)
	{
		return stringTable.find(id.token.text) != stringTable.end();
	}

	void add(Identifier id) {
		stringTable.insert(id.token.text);
	}
};

struct Symbol
{
	using Value = brick::types::Union<std::string, int>;

	Identifier identifier;
	std::size_t offset;
	bool global;
	//type

	Symbol(Identifier identifier, std::size_t offset, bool global) : identifier(identifier), offset(offset), global(global) {}
};

struct Scope
{
	std::unordered_map<int, Symbol> symbolTable;
	std::shared_ptr<Scope> parent;

	bool add(IdNumber n, Identifier id, std::size_t offset, bool global) {
		auto val = symbolTable.insert(std::make_pair(n.value, Symbol(id, offset, global)));
		return val.second;
	}

	bool exists(IdNumber n)	{
		bool found = symbolTable.find(n.value) != symbolTable.end();
		if (!found && parent != nullptr) {
			return parent->exists(n);
		}
		return found;
	}

	Symbol getSymbol(IdNumber n) {
		auto symbol = symbolTable.find(n.value);
		if (symbol == symbolTable.end() && parent != nullptr) {
			return parent->getSymbol(n);
		}
		return symbol->second;
	}

	/*Symbol::Value getValue(IdNumber n) {
		auto symbol = symbolTable.find(n.value);
		if (symbol == symbolTable.end() && parent != nullptr) {
			return parent->getValue(n);
		}
		return symbol->second.value;
	}

	void setValue(IdNumber n, Symbol::Value value) {
		auto symbol = symbolTable.find(n.value);
		if (symbol == symbolTable.end() && parent != nullptr) {
			parent->setValue(n, value);
			return;
		}
		symbol->second.value = value;
	}*/
};