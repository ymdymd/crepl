#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <memory>

#include <assert.h>

//=============================================================================
// util
#ifdef _DEBUG
#define FUNCTION_CALL_TRACE() std::cout << __FUNCTION__ << std::endl
#else
#define FUNCTION_CALL_TRACE() 
#endif

//-----------------------------------------------------------------------------
// error
void Error(const std::string& msg) {
	std::cerr << msg;
}

//=============================================================================
// lexer

//-----------------------------------------------------------------------------
// token type definition
enum TokenType {
	EOL = 0,		// End Of Line must be 0.
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

//-----------------------------------------------------------------------------
// token struct
struct Token {
	int type;			//token type
	std::string str;	//token string
	Token(void) : type(EOL), str("") {};
	Token(int _type, std::string _str) : type(_type), str(_str) {};
};


//-----------------------------------------------------------------------------
// Lexer
// lineの文字列をtokenに分割する。
std::list<Token> lexer(std::string line) {
	std::list<Token> tokens;

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

	tokens.push_back(Token(EOL,std::string("")));
	return tokens;
}

//=============================================================================
// AST (Abstract Syntax Tree)

//-----------------------------------------------------------------------------
// ExprAST - Base class for all expression nodes.
// ExprAST - 全ての式ノードの基底クラス。
class ExprAST {
public:
	virtual ~ExprAST() {}
	virtual int eval(void) { return 0; }
};
//-----------------------------------------------------------------------------
// IntegerExprAST - Expression class for integer literals like "1".
// IntegerExprAST - "1"のような整数数値リテラルのための式クラス。
class IntegerExprAST : public ExprAST {
	int Val;
public:
	IntegerExprAST(int val) : Val(val) {}
	virtual int eval(void) { return Val; }
};

//-----------------------------------------------------------------------------
// VariableExprAST - Expression class for referencing a variable, like "a".
// VariableExprAST - "a"のような変数を参照するための式クラス。
class VariableExprAST : public ExprAST {
	std::string Name;
public:
	VariableExprAST(const std::string &Name) : Name(Name) {}
//	virtual int eval(void) { return Val; }
};

//-----------------------------------------------------------------------------
// BinaryExprAST - Expression class for a binary operator.
// BinaryExprAST - 二項演算子のための式クラス。
class BinaryExprAST : public ExprAST {
	int Op;					//opcode
	std::unique_ptr<ExprAST> LHS, RHS;
public:
	BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS,
		std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	virtual int eval(void) {
		switch (Op) {
		case(ADD): return LHS->eval() + RHS->eval();
		case(SUB): return LHS->eval() - RHS->eval();
		case(MUL): return LHS->eval() * RHS->eval();
		case(DIV): return LHS->eval() / RHS->eval();
		}
		return 0;
	}
};


//=============================================================================
// Parser

static std::unique_ptr<ExprAST> primary_expression(std::list<Token>& tokens);


/*-----------------------------------------------------------------------------
integer_expression (terminate)
: number 
*/
static std::unique_ptr<ExprAST> integer_expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	assert(tokens.front().type == IMM);
	int value = std::stoi(tokens.front().str);
	auto Result = std::make_unique<IntegerExprAST>(value);
	tokens.pop_front();											// consume the number
	return std::move(Result);
}


/*-----------------------------------------------------------------------------
multiplicative_expression
: primary_expression
| primary_expression MUL primary_expression
| primary_expression DIV primary_expression
*/
static std::unique_ptr<ExprAST> multiplicative_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = primary_expression(tokens);
	if (!LHS) return nullptr;
	int op = tokens.front().type;
	if (op == MUL || op == DIV) {
		tokens.pop_front();	//eat op
		auto RHS = primary_expression(tokens);
		if (!RHS) 		return nullptr;
		return multiplicative_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}


/*-----------------------------------------------------------------------------
additive_expression
: multiplicative_expression
| multiplicative_expression ADD multiplicative_expression
| multiplicative_expression SUB multiplicative_expression
*/
static std::unique_ptr<ExprAST> additive_expression(std::list<Token>& tokens,std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = multiplicative_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == ADD || op == SUB) {
		tokens.pop_front();	//eat op
		auto RHS = multiplicative_expression(tokens,nullptr);
		if (!RHS) 		return nullptr;
		return additive_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
expression
: additive_expression
*/
static std::unique_ptr<ExprAST> expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	return additive_expression(tokens, nullptr);
}


/*-----------------------------------------------------------------------------
primary_expression
: integer_expression　(terminate)
| PARL expression PARR
*/
static std::unique_ptr<ExprAST> primary_expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	switch (tokens.front().type) {
	default:
		Error("unknown token when expecting an expression");
		return nullptr;
	case IMM:
		return integer_expression(tokens);
	case PARL: {
		tokens.pop_front();		// eat (.
		auto V = expression(tokens);	// expression
		if (!V)	return nullptr;
		//副次式を解析した後、”)”の出現がない可能性がある。
		if (tokens.front().type != PARR) {
			Error("expected ')'");
			return nullptr;
		}
		tokens.pop_front();		// eat ).
		return V;
	}
	}
}

//=============================================================================
// evalute expr_str
int eval(const std::string expr_str) {
	return expression(lexer(expr_str))->eval();
}



//=============================================================================
//test & main
#include <gtest/gtest.h>

#define TO_STR(...) #__VA_ARGS__
#define TEST_(expr) ASSERT_EQ( (expr), eval(TO_STR(expr)))

TEST(test, eval)
{
	ASSERT_EQ(1+1, eval("1+1"));
	ASSERT_EQ(1 + 1, eval("1 + 1"));
	TEST_(1 + 2 * 3);
	TEST_(1 * 2 + 3);
	TEST_(1 * (2 + 3));
	TEST_((1 * 2) + 3);
	TEST_(1 + 2 * 3 + 4 * 5 + 6 * 7 + 8 * 9);
	TEST_((1 + 2) * (3 + 4) * (5 + 6) * (7 + 8) * 9);
	TEST_((1 + 2 * 3) + (4 * 5 + 6) * (7 + 8 * 9));
	TEST_(((1 + 2 )* 3) + (4 * (5 + 6)) * ((7 + 8) * 9));
}


int main(int argc, char** argv)
{
	//	std::string line = "1223+123";
	//std::string line = "1223 + 123 * (3 + 3) && 1+3>>3";
	//std::string line = " 123+33*44";
	std::string line = "1 * 2 + 3";

	std::list<Token> tokens = lexer(line);

	std::cout << line << std::endl;
	for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
//		std::cout << itr->type << ":" << itr->str << std::endl;
		std::cout << itr->str << std::endl;
	}

	auto expr = expression(tokens);
	std::cout << expr->eval() << std::endl;

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

