#pragma once
#include <string>
#include "ast.h"
#include "lexer.h"

struct BadParse
{
	Token found;
	std::string expected;
	BadParse(Token t, std::string e) : found(t), expected(e) {}
};

static inline std::ostream &operator<<(std::ostream &out, BadParse bp)
{
	return out << "parse error at " << bp.found << ", expected " << bp.expected;
}

struct Parser
{
	Lexer lexer;
	Token token;

	Parser(const char *file) : lexer(file), token(Token::Eof, "", 0) {}

	template<typename T>
	Ptr<Expression> make_expr(T expr)
	{
		return std::make_shared<Expression>(expr);
	}

	void shift()
	{
		token = lexer.next();
	}

	void fail(std::string err)
	{
		throw BadParse(token, err);
	};

	Toplevel toplevel()
	{
		Toplevel result;
		while (true)
		{
			shift();
			if (token.category == Token::Eof) {
				return result;
			}
			result.globals.push_back(global());
		}
	}

	Global global()
	{
		if (token.category == Token::Func) {
			return func();
		}
		if (token.category == Token::Var) {
			return var();
		}
		fail("func or var");
	}

	Func func()
	{
		if (token.category != Token::Func) {
			fail("func");
		}
		shift();
		Func f;
		f.name = identifier();
		if (!isupper(f.name.token.text[0])) {
			fail("first letter to be uppercase");
		}

		shift();
		if (token.category != Token::ParentOpen) {
			fail("(");
		}

		f.parameters = arguments();

		shift();
		if (token.category != Token::ParentClose) {
			fail(")");
		}
		shift();

		f.body = block();
		return f;
	}

	Var var()
	{
		if (token.category != Token::Var) {
			fail("var");
		}
		shift();
		Var v;
		v.name = identifier();
		if (!islower(v.name.token.text[0])) {
			fail("first letter to be lowercase");
		}

		if (lexer.peek().category != Token::ParentClose) {
			shift();
			v.value = expressionOperCheck();
		}

		return v;
	}

	Identifier identifier()
	{
		if (token.category != Token::Identifier) {
			fail("identifier");
		}
		Identifier id;
		id.token = token;
		return id;
	}

	Ptr<Expression> expression()
	{
		bool parenthesis = false;
		if (token.category == Token::ParentOpen) {
			parenthesis = true;
			shift();
		}

		auto expr = (token.category == Token::ParentOpen) ?
					expression() : make_expr(expressionBase());

		if (parenthesis) {
			shift();
			if (token.category != Token::ParentClose) {
				fail(")");
			}
		}

		return expr;
	}

	Expression expressionBase()
	{
		if (token.category == Token::StringLit) {
			return Expression(token.text);
		}
		if (token.category == Token::NumericLit) {
			return Expression(std::stoi(token.text));
		} 
		if (lexer.isOperator(token.text)) {
			return Expression(oper());
		}
		if (token.category == Token::Identifier) {
			if (isupper(token.text[0])) {
				return Expression(call());
			}
			return Expression(identifier());
		}

		fail("string/numeric literal, operator, identifier or function call");
	}

	Operator oper()
	{
		if (!lexer.isOperator(token.text)) {
			fail("operator");
		}
		Operator op;
		op.token = token;
		op.operands = operands();
		return op;
	}

	Call call()
	{
		Call c;
		c.function = identifier();
		shift();

		if (token.category != Token::ParentOpen) {
			fail("(");
		}

		c.operands = operands();
		shift();
		if (token.category != Token::ParentClose) {
			fail(")");
		}
		return c;
	}

	std::vector<Ptr<Expression>> operands()
	{
		std::vector<Ptr<Expression>> opers;
		while (lexer.peek().category != Token::ParentClose) {
			shift();
			opers.push_back(expressionOperCheck());
		}
		return opers;
	}

	std::vector<Identifier> arguments()
	{
		std::vector<Identifier> args;
		while (lexer.peek().category != Token::ParentClose) {
			shift();
			args.push_back(identifier());
		}

		return args;
	}

	Ptr<Block> block()
	{
		if (token.category != Token::ParentOpen) {
			fail("(");
		}
		shift();

		Block b;
		while (token.category != Token::ParentClose) {
			b.statements.push_back(std::make_shared<Statement>(statement()));
			shift();
		}

		if (token.category != Token::ParentClose) {
			fail(")");
		}

		return std::make_shared<Block>(b);
	}

	Statement statement()
	{
		if (token.category != Token::ParentOpen) {
			fail("(");
		}
		shift();

		auto st = (token.category == Token::ParentOpen) ?
			statement() : statementBase();

		shift();
		if (token.category != Token::ParentClose) {
			fail(")");
		}
		return st;
	}

	Statement statementBase()
	{
		Statement st;

		if (token.category == Token::Var) {
			st = var();
		}
		else if (token.category == Token::If) {
			st = ifStatement();
		}
		else if (token.category == Token::While) {
			st = whileStatement();
		}
		else if (token.category == Token::For) {
			st = forStatement();
		}
		else if (token.category == Token::Return) {
			st = returnStatement();
		}
		else if (lexer.isOperator(token.text)) {
			st = oper();
		}
		else if (isFunctionCall()) {
			st = call();
		}
		else {
			fail("var, if, while, for, return, operator or function call");
		}
		return st;
	}

	Ptr<Expression> condition()
	{
		if (token.category != Token::ParentOpen) {
			fail("(");
		}
		shift();

		auto cond = expression();

		shift();
		if (token.category != Token::ParentClose) {
			fail(")");
		}

		return cond;
	}

	If ifStatement()
	{
		if (token.category != Token::If) {
			fail("if");
		}
		shift();
		
		If i;
		i.condition = condition();
		shift();
		i.body = block();

		if (lexer.peek().category == Token::Else) {
			shift();
			shift();
			i.elseBody = block();
		}
		return i;
	}

	While whileStatement()
	{
		if (token.category != Token::While) {
			fail("while");
		}
		shift();

		While w;
		w.condition = condition();
		shift();
		w.body = block();

		return w;
	}

	For forStatement()
	{
		if (token.category != Token::For) {
			fail("for");
		}
		shift();
		if (token.category != Token::ParentOpen) {
			fail("(");
		}
		shift();
		if (token.category != Token::Var) {
			fail("var");
		}
		shift();
		
		For f;
		f.variable = identifier();
		if (!islower(f.variable.token.text[0])) {
			fail("first letter to be lowercase");
		}

		shift();
		if (token.category != Token::From) {
			fail("from");
		}
		shift();

		f.from = expressionOperCheck();

		shift();
		if (token.category == Token::To) {
			f.downto = false;
		}
		else if (token.category == Token::DownTo) {
			f.downto = true;
		}
		else {
			fail("to or downto");
		}
		shift();

		f.to = expressionOperCheck();

		shift();
		if (token.category != Token::ParentClose) {
			fail(")");
		}
		shift();

		f.body = block();
		return f;
	}

	Return returnStatement()
	{
		if (token.category != Token::Return) {
			fail("return");
		}
		shift();
		Return r;

		r.returnValue = expressionOperCheck();
		return r;
	}

protected:
	bool isFunctionCall()
	{
		return token.category == Token::Identifier && isupper(token.text[0]);
	}

	Ptr<Expression> expressionOperCheck()
	{
		if (lexer.isOperator(token.text)) {
			fail("( before operator");
		}
		return expression();
	}
};


//implement for:downto 