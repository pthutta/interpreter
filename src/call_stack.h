#pragma once
#include <memory>
#include <brick-types>
#include <stack>
#include "ast.h"

using brick::types::Union;

using Value = Union<std::string, int>;

struct CallStack {
	struct Frame {
		std::vector<Value> values;

		Frame(std::size_t size) : values(size) {}

		Value &operator[](std::size_t idx)
		{
			return values[idx];
		}

		const Value &operator[](std::size_t idx) const
		{
			return values[idx];
		}

		std::size_t size() const { return values.size(); }

		void resize(std::size_t size) {
			values.resize(size);
		}
	};

	std::stack<Frame> frames;

	Value &operator[](std::size_t idx)
	{
		return frames.top()[idx];
	}

	const Value &operator[](std::size_t idx) const
	{
		return frames.top()[idx];
	}

	void push(std::size_t size) {
		frames.push(Frame(size));
	}

	void push(Frame frame) {
		frames.push(frame);
	}

	void pop() {
		frames.pop();
	}

	std::size_t topSize() {
		return frames.top().size();
	}

	void resizeTopFrame(std::size_t size) {
		frames.top().resize(size);
	}
};


struct Environment {
	StringTable *stringTable;
	CallStack callStack;
	std::vector<Value> globals;

	void start(StringTable *stringTable, std::size_t globalsSize)
	{
		this->stringTable = stringTable;
		globals.resize(globalsSize);
	}

	Value &var(Identifier &i, Ptr<Scope> currentScope)
	{
		auto symbol = getSymbol(i, currentScope);
		if (symbol.global) {
			return globals[symbol.offset];
		}
		return callStack[symbol.offset];
	}

	const Value &var(Identifier &i, Ptr<Scope> currentScope) const
	{
		auto symbol = getSymbol(i, currentScope);
		if (symbol.global) {
			return globals[symbol.offset];
		}
		return callStack[symbol.offset];
	}

	Symbol getSymbol(Identifier &i, Ptr<Scope> currentScope) const
	{
		auto n = stringTable->get(i);
		return currentScope->getSymbol(n);
	}

	void pushFrame(std::size_t size)
	{
		callStack.push(size);
	}

	void pushFrame(CallStack::Frame frame)
	{
		callStack.push(frame);
	}

	void popFrame()
	{
		callStack.pop();
	}

	std::size_t topFrameSize()
	{
		return callStack.topSize();
	}

	Value &operator[](std::size_t idx)
	{
		return callStack[idx];
	}

	const Value &operator[](std::size_t idx) const
	{
		return callStack[idx];
	}

	void resizeTopFrame(std::size_t size) {
		callStack.resizeTopFrame(size);
	}
};

Value operator+(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() + rhs.get<int>();
	}
	if (lhs.is<std::string>() && rhs.is<std::string>()) {
		return lhs.get<std::string>() + rhs.get<std::string>();
	}
	//fail();
}

Value operator-(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() - rhs.get<int>();
	}
	//fail();
}

Value operator*(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() * rhs.get<int>();
	}
	//fail();
}

Value operator/(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() / rhs.get<int>();
	}
	//fail();
}

Value operator%(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() % rhs.get<int>();
	}
	//fail();
}

/*bool operator==(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() == rhs.get<int>();
	}
	if (lhs.is<std::string>() && rhs.is<std::string>()) {
		return lhs.get<std::string>() == rhs.get<std::string>();
	}
	if (lhs.is<std::string>() && rhs.is<int>()) {
		return lhs.get<std::string>().size() == rhs.get<int>();
	}
	if (lhs.is<int>() && rhs.is<std::string>()) {
		return rhs.get<std::string>().size() == lhs.get<int>();
	}
	//fail();
}*/

bool operator!=(const Value &lhs, const Value &rhs) {
	return !(lhs == rhs);
}

/*bool operator<(const Value &lhs, const Value &rhs) {
	if (lhs.is<int>() && rhs.is<int>()) {
		return lhs.get<int>() < rhs.get<int>();
	}
	if (lhs.is<std::string>() && rhs.is<std::string>()) {
		return lhs.get<std::string>() < rhs.get<std::string>();
	}
	if (lhs.is<std::string>() && rhs.is<int>()) {
		return lhs.get<std::string>().size() < rhs.get<int>();
	}
	if (lhs.is<int>() && rhs.is<std::string>()) {
		return rhs.get<std::string>().size() < lhs.get<int>();
	}
	//fail();
}*/

bool operator>(const Value &lhs, const Value &rhs) {
	return rhs < lhs;
}

bool operator<=(const Value &lhs, const Value &rhs) {
	return !(rhs < lhs);
}

bool operator>=(const Value &lhs, const Value &rhs) {
	return !(lhs < rhs);
}