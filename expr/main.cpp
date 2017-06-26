#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <memory>

#include <assert.h>

enum TokenType {
	INVALID = 0,
	ADD = 1,
	SUB,
	MUL,
	DIV,
	AND,
	OR,
	XOR,
	INV,
	LAND,
	LOR,
	NOT,
	SFTL,
	SFTR,
	EQ,
	NE,
	LT,
	LE,
	GT,
	GE,
	PARL,
	PARR,
	IMM,
};

struct Token {
	int type;			//token type
	std::string str;	//token string
};

#if 1

void lex(std::string line, std::list<Token>& tokens) {

	std::vector<Token> keywords = {
		{ SFTL,		R"(^\<\<)" },
		{ SFTR,		R"(^\>\>)" },
		{ EQ,		R"(^\=\=)" },
		{ NE,		R"(^\!\=)" },
		{ LT,		R"(^\<)" },
		{ LE,		R"(^\<\=)" },
		{ GT,		R"(^\>)" },
		{ GE,		R"(^\>\=)" },
		{ LAND,		R"(^\&\&)" },
		{ LOR,		R"(^\|\|)" },
		{ ADD,		R"(^\+)" },
		{ SUB,		R"(^\-)" },
		{ MUL,		R"(^\*)" },
		{ DIV,		R"(^\/)" },
		{ AND,		R"(^\&)" },
		{ OR,		R"(^\|)" },
		{ XOR,		R"(^\^)" },
		{ INV,		R"(^\~)" },
		{ NOT,		R"(^\!)" },
		{ PARL,		R"(^\()" },
		{ PARR,		R"(^\))" },
		{ IMM,		R"(^[0-9]+)" },
	};

	while (line.length()) {
		std::smatch m;

		//skip white spcae
		if (regex_search(line, m, std::regex(R"(^[ \t]+)"))) {
			line.erase(line.begin(), line.begin() + m[0].length());
			continue;
		}

		Token token;
		size_t i;
		for (i = 0; i < keywords.size(); i++) {
			if (regex_search(line, m, std::regex(keywords[i].str))) {
				token.str  = m[0];
				token.type = keywords[i].type;
				line.erase(line.begin(), line.begin() + m[0].length());
				break;
			}
		}

		if (token.type) {
			tokens.push_back(token);
		}
		else {	//見つからなかった場合は、残りをすべてtokensに入れる
			token.str = line;
			token.type = keywords[i].type;
			tokens.push_back(token);
			break;
		}
	}
}

// ExprAST - Base class for all expression nodes.
// ExprAST - 全ての式ノードの基底クラス。
class ExprAST {
public:
	virtual ~ExprAST() {}
};

// NumberExprAST - Expression class for numeric literals like "1.0".
// NumberExprAST - "1.0"のような数値リテラルのための式クラス。
class NumberExprAST : public ExprAST {
	int Val;
public:
	NumberExprAST(int val) : Val(val) {}
};

// VariableExprAST - Expression class for referencing a variable, like "a".
// VariableExprAST - "a"のような変数を参照するための式クラス。
class VariableExprAST : public ExprAST {
	std::string Name;

public:
	VariableExprAST(const std::string &Name) : Name(Name) {}
};

// BinaryExprAST - Expression class for a binary operator.
// BinaryExprAST - 二項演算子のための式クラス。
class BinaryExprAST : public ExprAST {
	int Op;					//opcode
	std::unique_ptr<ExprAST> LHS, RHS;

public:
	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

void Error(const std::string& msg) {
	std::cerr << msg;
}


static std::unique_ptr<ExprAST> ParsePrimaryExpr(std::list<Token>& tokens);



/*
multiplicative_expression
: primary_expression
| multiplicative_expression MUL primary_expression
| multiplicative_expression DIV primary_expression
*/
static std::unique_ptr<ExprAST> MultiplicativeExpression(std::list<Token>& tokens) {
	std::cout << __FUNCTION__ << std::endl;
	auto V = ParsePrimaryExpr(tokens);
	if (V) return V;
	
	auto LHS = MultiplicativeExpression(tokens);
	if (!LHS) 		return nullptr;

	tokens.pop_front();

	int op = tokens.front().type;
	if (op == MUL || op == DIV) {
		tokens.pop_front();	//eat *
		auto RHS = ParsePrimaryExpr(tokens);
		if (!RHS) 		return nullptr;
		tokens.pop_front();	//eat rhs
							// Merge LHS/RHS.
		return std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
	}
	return nullptr;
}



/*
additive_expression
: multiplicative_expression
| additive_expression ADD multiplicative_expression
| additive_expression SUB multiplicative_expression
*/
static std::unique_ptr<ExprAST> AdditiveExpression(std::list<Token>& tokens) {
	std::cout << __FUNCTION__ << std::endl;
	auto V = MultiplicativeExpression(tokens);
	if (V) return V;

	auto LHS = AdditiveExpression(tokens);
	if (!LHS) 		return nullptr;

	tokens.pop_front();

	int op = tokens.front().type;
	if (op == ADD || op == SUB) {
		tokens.pop_front();	//eat *
		auto RHS = MultiplicativeExpression(tokens);
		if (!RHS) 		return nullptr;
		tokens.pop_front();	//eat rhs
							// Merge LHS/RHS.
		return std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS));
	}
	return nullptr;
}
/*
expression
: conditional_expression
*/
static std::unique_ptr<ExprAST> ParseExpression(std::list<Token>& tokens) {
	std::cout << __FUNCTION__ << std::endl;
	//プライマリ式ではじまり...
	auto LHS = ParsePrimaryExpr(tokens);
	if (!LHS)return nullptr;


	return AdditiveExpression(tokens);
}



// numberexpr ::= number
// 現在のトークンがtok_numberの場合にこの関数が呼び出される事を想定している。
// この関数は現在の数値（NumVal）を読み取り、NumberExprASTノードを生成し、字句解析器を次のトークンへと進め、そして最後にノードを返す。
static std::unique_ptr<ExprAST> ParseNumberExpr(std::list<Token>& tokens) {
	std::cout << __FUNCTION__ << std::endl;
	assert(tokens.front().type == IMM);
	int value = std::stoi(tokens.front().str);
	auto Result = std::make_unique<NumberExprAST>(value);
	tokens.pop_front();											// consume the number
	return std::move(Result);
}

// parenexpr ::= '(' expression ')'
// この関数は、現在のトークンが”(“の場合に呼ばれることを想定している
// 再帰的にParseExpressionを呼び出している。
// 丸括弧そのものはASTノードの構築を行わないことに注意
static std::unique_ptr<ExprAST> ParseParenExpr(std::list<Token>& tokens) {
	std::cout << __FUNCTION__ << std::endl;
	assert(tokens.front().type == PARL);
	tokens.pop_front();		// eat (.
	auto V = ParseExpression(tokens);	// expression
	if (!V)
		return nullptr;

	//副次式を解析した後、”)”の出現がない可能性がある。
	if (tokens.front().type != PARR) {
		Error("expected ')'");
		return nullptr;
	}
	tokens.pop_front();		// eat ).
	return V;
}



/*
primary_expression
: INT_LITERAL
| PARL expression PARR
*/

// ”プライマリ”式
static std::unique_ptr<ExprAST> ParsePrimaryExpr(std::list<Token>& tokens) {
	std::cout << __FUNCTION__ << std::endl;
	switch (tokens.front().type) {
	default:
		Error("unknown token when expecting an expression");
		return nullptr;
//	case tok_identifier:
//		return ParseIdentifierExpr();
	case IMM:
		return ParseNumberExpr(tokens);
	case PARL:
		return ParseParenExpr(tokens);
	}
}







int main(void)
{
	//	std::string line = "1223+123";
	//std::string line = "1223 + 123 * (3 + 3) && 1+3>>3";
	//std::string line = " 123+33*44";
	std::string line = "1+2";

	std::list<Token> tokens;
	lex(line, tokens);

	std::cout << line << std::endl;
	for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
//		std::cout << itr->type << ":" << itr->str << std::endl;
		std::cout << itr->str << std::endl;
	}

//	ParseExpression(tokens);


}

#else
void lex(std::string line, std::list<std::string>& tokens) {

	std::vector<std::string> keywords = {
		R"(^[\+\-\*\%])",
		R"(^((\<\<)|(>\>)))",
		R"(^((\&\&)|(\|\|)|(\&)|(\|)|(\^)))",
		R"(((\<)|(\>)|(\<\=)|(\>\=)|(\=\=)|(\!\=))",
		R"(^[0-9]+)",
		R"(^[a-zA-Z_][a-zA-Z_]+)",
	};

	while (line.length()) {
		std::smatch m;

		//skip white spcae
		if (regex_search(line, m , std::regex(R"(^[ \t]+)"))) {
			line.erase(line.begin(), line.begin() + m[0].length());
			continue;
		}

		std::string token;
		size_t i;
		for (i = 0; i < keywords.size(); i++) {
			if (regex_search(line, m, std::regex(keywords[i]))) {
				token = m[0];
				line.erase(line.begin(), line.begin() + m[0].length());
				break;
			}
		}
		if (token.empty()) {	//見つからなかった場合は、残りをすべてtokensに入れる
			tokens.push_back(line);
			break;
		}
		tokens.push_back(token);
	}
}



int main(void)
{
//	std::string line = "1223+123";
	std::string line = "1223 + 123 * 3 + 3 && 1+3>>3";

	std::list<std::string> tokens;
	lex(line, tokens);

	std::cout << line << std::endl;
	for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
		std::cout << *itr << std::endl;
	}
}

#endif
