#pragma once
#include <brick-types>
#include <functional>
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <stack>
#include "analyzer.h"
#include "ast.h"
#include "call_stack.h"


struct RuntimeError
{
	std::string message;
	RuntimeError(std::string m) : message(m) {}
};

static inline std::ostream &operator<<(std::ostream &out, RuntimeError re)
{
	out << re.message;
	return out;
}

struct Evaluator {
	Analyzer analyzer;
	Toplevel toplevel;
	Environment environment;
	bool returned;

	Evaluator(const char *file) : analyzer(file) {}

	void evalAndPrint()
	{
		for (auto r : eval()) {
			print(r);
		}
	}

	std::vector<Value> eval()
	{
		std::vector<Value> results;
		toplevel = analyzer.toplevel();
		environment.start(&analyzer.parser.stringTable, toplevel.frameSize);
		for (auto &global : toplevel.globals) {
			global.match([&](Call c) {
				results.push_back(eval(*make_expr(c), toplevel.scope));
			},
			[&](Var v) {
				eval(v, toplevel.scope);
			});
		}
		return results;
	}

	void eval(If &i, Ptr<Scope> currentScope)
	{
		if (convert(eval(*i.condition, currentScope))) {
			eval(*i.body, i.body->scope);
		} else if (i.elseBody != nullptr) {
			eval(*i.elseBody, i.elseBody->scope);
		}
	}

	void eval(While &w, Ptr<Scope> currentScope)
	{
		while (convert(eval(*w.condition, currentScope))) {
			eval(*w.body, w.body->scope);
			if (returned) {
				break;
			}
		}
	}

	void eval(For &f, Ptr<Scope> currentScope)
	{
		auto from = eval(*f.from, currentScope);
		//TODO check if "from" is int

		auto to = eval(*f.to, currentScope);
		//TODO check if "to" is int

		int i;
		auto symbol = environment.getSymbol(f.variable, f.body->scope);
		std::vector<std::function<void()>> functors{[&] { i++; }, [&] { i--; }};
		std::vector<std::function<bool()>> comps{[&] { return i <= to.get<int>(); }, [&] { return i >= to.get<int>(); }};

		for (i = from.get<int>(); comps[static_cast<int>(f.downto)]() ; functors[static_cast<int>(f.downto)]()) {
			environment[symbol.offset] = i;
			eval(*f.body, f.body->scope);
			if (returned) {
				break;
			}
		}
	}

	Value eval(Expression &ex, Ptr<Scope> currentScope)
	{
		Expression toEval = ex;
		std::stack<std::pair<Expression, int>> opers;
		std::stack<Atom> values;

		while (true) {
			if (toEval.is<Atom>()) {
				values.push(std::move(toEval.get<Atom>()));

				if (opers.size() == 0) {
					return eval(values.top(), currentScope);
				}

				--opers.top().second;
				if (opers.top().second == 0) {
					auto count = getOperandCount(opers.top().first);
					std::vector<Atom> operands(count);

					for (int i = count-1; i >= 0; i--) {
						operands[i] = values.top();
						values.pop();
					}

					toEval = Expression(ExprBase(eval(opers.top().first, std::move(operands), currentScope)));

					opers.pop();
				} else {
					auto o = getNextOperand(opers.top());
					toEval = (o.is<Atom>()) ? Expression(ExprBase(eval(o.get<Atom>(), currentScope))) : o;
				}
			}
			else {
				std::size_t size;

				if (toEval.is<Operator>()) {
					size = toEval.get<Operator>().operands.size();

					if (size == 0) {
						fail(toEval.get<Operator>().token, " did not get any operand" );
					}
				} else if (toEval.is<Call>()) {
					size = toEval.get<Call>().operands.size();
				}

				if (toEval.is<Call>() && size == 0) {
					toEval = Expression(ExprBase(eval(toEval, std::vector<Atom>(), currentScope)));
				}
				else {
					opers.push(std::make_pair(toEval, size));
					auto o = getNextOperand(opers.top());
					toEval = (o.is<Atom>() && !(toEval.is<Operator>() && toEval.get<Operator>().token.category == Token::Assign))
					         ? Expression(ExprBase(eval(o.get<Atom>(), currentScope))) : o;
				}
			}
		}
	}

	Value eval(Expression &ex, std::vector<Atom> &&operands, Ptr<Scope> currentScope) {
		if (ex.is<Operator>()) {
			return eval(ex.get<Operator>().token, operands, currentScope);
		} else if (ex.is<Call>()) {
			return eval(ex.get<Call>(), operands, currentScope);
		}
	}

	Value eval(Token oper, std::vector<Atom> &operands, Ptr<Scope> currentScope)
	{
		if (oper.category == Token::Assign) {
			if (operands.size() != 2) {
				fail(oper, " requires two operands");
			}
			if (!operands[0].is<Identifier>()) {
				fail(oper, " requires first operand to be variable identifier");
			}
			auto value = eval(operands[1], currentScope);
			auto v = value.get<int>();
			environment.var(operands[0].get<Identifier>(), currentScope) = value;
			return value;
		}
		if (oper.category == Token::Not && operands.size() != 1) {
			fail(oper, " requires one operand");
		}

		std::vector<Value> values;
		std::for_each(operands.begin(), operands.end(), [&](Atom &a) { values.push_back(eval(a, currentScope)); });
		return operators(oper, values, currentScope);
	}

	Value eval(Call &call, std::vector<Atom> &operands, Ptr<Scope> currentScope)
	{
		auto func = getFunction(call.function);

		if (func.parameters.size() != operands.size()) {
			fail(call.function.token, ": the number of given arguments is different than number of required arguments");
		}

		if (call.isTail) {
			environment.resizeTopFrame(func.frameSize);
		}
		else {
			environment.pushFrame(func.frameSize);
		}

		for (int i = 0; i < operands.size(); i++) {
			environment[i] = eval(operands[i], currentScope);
		}

		returned = false;
		environment[environment.topFrameSize() - 1] = 0;

		eval(*func.body, func.body->scope);

		auto value = environment[environment.topFrameSize() - 1];

		if (!call.isTail) {
			environment.popFrame();
		}
		return value;
	}

	Value eval(Atom &a, Ptr<Scope> currentScope)
	{
		Value value;
		a.match([&](Identifier &i) {
			value = environment.var(i, currentScope);
		}, [&](Literal &l) { value = l; });
		return value;
	}

	void eval(Var &v, Ptr<Scope> currentScope)
	{
		if (v.value != nullptr) {
			environment.var(v.name, currentScope) = eval(*v.value, currentScope);
		} else {
			environment.var(v.name, currentScope) = 0;
		}
	}

	void eval(Return &r, Ptr<Scope> currentScope)
	{
		auto value = eval(*r.returnValue, currentScope);
		environment[environment.topFrameSize() - 1] = value;
		returned = true;
	}

	void eval(Block &b, Ptr<Scope> currentScope)
	{
		for (auto statement : b.statements) {
			statement->match([&](Var &v) { eval(v, currentScope); },
			                 [&](If &i) { eval(i, currentScope); },
			                 [&](While &w) { eval(w, currentScope); },
			                 [&](For &f) { eval(f, currentScope); },
			                 [&](Return &r) { eval(r, currentScope); },
			                 [&](Call &c) { eval(*make_expr(c), currentScope); },
			                 [&](Operator &o) { eval(*make_expr(o), currentScope); });
			if (returned) {
				break;
			}
		}
	}

private:
	template <typename T>
	void fail(T t, std::string msg)
	{
		std::stringstream s;
		s << t;
		throw RuntimeError(s.str() + msg);
	}

	bool convert(Value val)
	{
		if (val.is<int>()) {
			return val.get<int>() != 0;
		} else if (val.is<std::string>()) {
			return !val.get<std::string>().empty();
		}
	}

	void print(Value val)
	{
		std::cout << val << std::endl;
	}

	void print(Identifier &i, Ptr<Scope> currentScope)
	{
		std::cout << environment.var(i, currentScope) << std::endl;
	}

	Func getFunction(Identifier &i)
	{
		for (auto &global : toplevel.globals) {
			if (global.is<Func>() && global.get<Func>().name.token.text == i.token.text) {
				return global.get<Func>();
			}
		}
	}

	Value operators(Token oper, std::vector<Value> &operands, Ptr<Scope> currentScope)
	{
		switch (oper.category) {
			case Token::Plus:
				return std::accumulate(operands.begin(), operands.end(), Value(0),
				                       [](Value &lhs, Value &rhs) { return lhs + rhs; });

			case Token::Minus:
				if (operands.size() == 1) {
					return 0 - operands.front();
				}
				else {
					return std::accumulate(operands.begin() + 1, operands.end(), operands.front(),
					                       [](Value &lhs, Value &rhs) { return lhs - rhs; });
				}

			case Token::Times:
				return std::accumulate(operands.begin(), operands.end(), Value(1),
				                       [](Value &lhs, Value &rhs) { return lhs * rhs; });

			case Token::Slash:
				if (operands.size() == 1) {
					return operands.front();
				}
				else {
					return std::accumulate(operands.begin() + 1, operands.end(), operands.front(),
					                       [](Value &lhs, Value &rhs) { return lhs / rhs; });
				}

			case Token::Modulo:
				if (operands.size() == 1) {
					return operands.front();
				}
				else {
					return std::accumulate(operands.begin() + 1, operands.end(), operands.front(),
					                       [](Value &lhs, Value &rhs) { return lhs % rhs; });
				}

			case Token::And:
				return  std::find_if(operands.begin(), operands.end(), [](Value &v) {
					return v != 0;
				}) == operands.end();

			case Token::Or:
				return std::find_if(operands.begin(), operands.end(), [](Value &v) {
					return v != 0;
				}) != operands.end();

			case Token::Not:
				return !convert(operands.front());

			case Token::Eq:
				return find_if(operands.begin() + 1, operands.end(), [&](Value &v) {
					return operands.front() != v;
				}) == operands.end();

			case Token::NotEq:
				std::sort(operands.begin(), operands.end());
				return std::adjacent_find(operands.begin(), operands.end()) == operands.end();

			case Token::Less:
				return checkOrder(operands, [](Value &lhs, Value &rhs) { return lhs < rhs; });

			case Token::LessEq:
				return checkOrder(operands, [](Value &lhs, Value &rhs) { return lhs <= rhs; });

			case Token::Greater:
				return checkOrder(operands, [](Value &lhs, Value &rhs) { return lhs > rhs; });

			case Token::GreaterEq:
				return checkOrder(operands, [](Value &lhs, Value &rhs) { return lhs >= rhs; });

			default:
				break;
		}
	}

	template< typename Compare >
	bool checkOrder(std::vector<Value> &operands, Compare comp)
	{
		for (int i = 0; i < operands.size() - 1; i++) {
			if (!comp(operands[i], operands[i+1])) {
				return false;
			}
		}
		return true;
	}

	std::size_t getOperandCount(Expression &ex)
	{
		if (ex.is<Operator>()) {
			auto &op = ex.get<Operator>();
			return op.operands.size();

		} else if (ex.is<Call>()) {
			auto &call = ex.get<Call>();
			return call.operands.size();
		}
	}

	Expression getNextOperand(std::pair<Expression, int> &oper)
	{
		auto &ex = oper.first;
		auto idx = oper.second;

		if (ex.is<Operator>()) {
			auto &op = ex.get<Operator>();
			auto i = op.operands.size() - idx;
			return *op.operands[i];

		} else if (ex.is<Call>()) {
			auto &call = ex.get<Call>();
			auto i = call.operands.size() - idx;
			return *call.operands[i];
		}
	}
};