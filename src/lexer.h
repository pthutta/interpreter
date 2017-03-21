#pragma once
#include <fstream>
#include <cctype>
#include <map>

struct Token
{
	enum Category
	{
		ParentOpen, ParentClose,
		StringLit, NumericLit, Identifier,
		If, Else, For, From, To, DownTo, While, Func, Var, Return,
		Plus, Minus, Times, Slash, Modulo, Assign,
		And, Or, Not,
		Eq, NotEq, Less, LessEq, Greater, GreaterEq,
		Error, Eof
	} category;

	std::string text;
	int line;

	Token(Category cat, std::string text, int line)
		: category(cat), text(text), line(line)
	{}
};

static const char *categoryNames[] = {
	"ParentOpen", "ParentClose",
	"StringLit", "NumericLit", "Identifier",
	"If", "Else", "For", "From", "To", "DownTo", "While", "Func", "Var", "Return",
	"Plus", "Minus", "Times", "Slash", "Modulo", "Assign",
	"And", "Or", "Not",
	"Eq", "NotEq", "Less", "LessEq", "Greater", "GreaterEq", 
	"Error", "Eof"
};

static inline std::ostream &operator<<(std::ostream &out, const Token token) {

	return out << "[" << categoryNames[token.category] << "] " << token.text << " at line " << token.line;
}

struct Lexer
{
	std::ifstream in;
	std::string buffer;
	char c;
	int line = 1;

	std::map< std::string, Token::Category > operators;
	std::map< std::string, Token::Category > keywords;

	Lexer(const char *f) : in(f, std::ios::binary)
	{
		std::map< std::string, Token::Category > op { 
			{ "+", Token::Plus }, { "-", Token::Minus }, { "*", Token::Times }, { "/", Token::Slash }, { "%", Token::Modulo }, { "=", Token::Assign },
			{ "&&", Token::And }, { "||", Token::Or }, { "!", Token::Not },
			{ "==", Token::Eq }, { "!=", Token::NotEq }, { "<", Token::Less }, { "<=", Token::LessEq }, { ">", Token::Greater },{ ">=", Token::GreaterEq }
		};
		operators = std::move(op);

		std::map< std::string, Token::Category > key {
			{ "if", Token::If }, { "else", Token::Else }, { "for", Token::For }, { "from", Token::From }, { "to", Token::To }, { "downto", Token::DownTo },
			{ "while", Token::While }, { "func", Token::Func }, { "var", Token::Var }, { "return", Token::Return }
		};
		keywords = std::move(key);
	}

	Token next()
	{
		whitespace();
		buffer += (c = in.get());

		if (c == EOF) {
			return Token(Token::Eof, "", line);
		}

		if (c == '(') {
			return shift(Token::ParentOpen);
		}
		if (c == ')') {
			return shift(Token::ParentClose);
		}

		if (std::isalpha(c)) {
			return identifier();
		}
		
		if (std::isdigit(c)) {
			return  numericLiteral();
		}

		if (c == '"') {
			return stringLiteral();
		}

		return operatorSymbol();
	}

	Token peek()
	{
		auto pos = in.tellg();
		auto oldLine = line;
		Token tok = next();
		in.clear();
		in.seekg(pos);
		line = oldLine;
		return tok;
	}

	bool isOperator(std::string text)
	{
		return operators.find(text) != operators.end();
	}

	bool isKeyword(std::string text)
	{
		return keywords.find(text) != keywords.end();
	}

protected:
	void whitespace()
	{
		while (std::isspace(c = in.get())) {
			if (c == '\n') {
				++line;
			}
		}
		in.unget();
	}

	Token shift(Token::Category c)
	{
		Token t(c, buffer, line);
		buffer.clear();
		return t;
	}

	Token identifier()
	{
		while (std::isalnum(c = in.get())) {
			buffer += c;
		}
		in.unget();
		if (isKeyword(buffer)) {
			return shift(keywords[buffer]);
		}
		return shift(Token::Identifier);
	}

	Token stringLiteral()
	{
		buffer.clear();
		while ((c = in.get()) != '"') {
			if (c == EOF) {
				return shift(Token::Error);
			}
			buffer += c;
		}

		return shift(Token::StringLit);
	}

	Token numericLiteral()
	{
		while (std::isdigit(c = in.get())) {
			buffer += c;
		}
		in.unget();

		return shift(Token::NumericLit);
	}

	Token operatorSymbol()
	{
		char next = in.peek();
		if (isOperator(buffer + next)) {
			buffer += in.get();
		}
		if (isOperator(buffer)) {
			return shift(operators[buffer]);
		}
		return shift(Token::Error);
	}
};
