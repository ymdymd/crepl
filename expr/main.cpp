#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <memory>
#include <map>

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
	SEMICOLON = 1,	// :
	COLON,			// ;
	QUESTION,		// ?
	ADD,			// +
	SUB,
	MUL,
	DIV,
	MOD,
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
	VAR,
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
		{ LE,		R"(^\<\=)" },
		{ GE,		R"(^\>\=)" },
		{ LAND,		R"(^\&\&)" },
		{ LOR,		R"(^\|\|)" },
		{ LT,		R"(^\<)" },
		{ GT,		R"(^\>)" },
		{ ADD,		R"(^\+)" },
		{ SUB,		R"(^\-)" },
		{ MUL,		R"(^\*)" },
		{ DIV,		R"(^\/)" },
		{ MOD,		R"(^\%)" },
		{ AND,		R"(^\&)" },
		{ OR,		R"(^\|)" },
		{ XOR,		R"(^\^)" },
		{ INV,		R"(^\~)" },
		{ NOT,		R"(^\!)" },
		{ PARL,		R"(^\()" },
		{ PARR,		R"(^\))" },
		{ SEMICOLON,R"(^\;)" },
		{ COLON,    R"(^\:)" },
		{ QUESTION, R"(^\?)" },
		{ IMM,		R"(^[0-9]+)" },
		{ VAR,		R"(^[a-zA-Z][a-zA-Z0-9]+)" },
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
	virtual int eval( int (*fp)(std::string&) = nullptr) { return 0; }
};
//-----------------------------------------------------------------------------
// IntegerExprAST - Expression class for integer literals like "1".
// IntegerExprAST - "1"のような整数数値リテラルのための式クラス。
class IntegerExprAST : public ExprAST {
	int Val;
public:
	IntegerExprAST(int val) : Val(val) {}
	virtual int eval(int(*fp)(std::string&) = nullptr) { return Val; }
};

//-----------------------------------------------------------------------------
// VariableExprAST - Expression class for referencing a variable, like "a".
// VariableExprAST - "a"のような変数を参照するための式クラス。
class VariableExprAST : public ExprAST {
	std::string Name;
public:
	VariableExprAST(const std::string &Name) : Name(Name) {}
	virtual int eval(int(*fp)(std::string&) = nullptr) { return (fp)?fp(Name):0; }
};

//-----------------------------------------------------------------------------
// UnaryExprAST - Expression class for a unary operator.
class UnaryExprAST : public ExprAST {
	int Op;					//opcode
	std::unique_ptr<ExprAST> RHS;
public:
	UnaryExprAST(int Op, std::unique_ptr<ExprAST> RHS) 
		: Op(Op), RHS(std::move(RHS)) {}
	virtual int eval(int(*fp)(std::string&) = nullptr) {
		switch (Op) {
		case(ADD): return +RHS->eval(fp);
		case(SUB): return -RHS->eval(fp);
		case(INV): return ~RHS->eval(fp);
		case(NOT): return !RHS->eval(fp);
		}
		return 0;
	}
};

//-----------------------------------------------------------------------------
// BinaryExprAST - Expression class for a binary operator.
// BinaryExprAST - 二項演算子のための式クラス。
class BinaryExprAST : public ExprAST {
	int Op;					//opcode
	std::unique_ptr<ExprAST> LHS, RHS;
public:
	BinaryExprAST(int Op, std::unique_ptr<ExprAST> LHS,	std::unique_ptr<ExprAST> RHS)
		: Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	virtual int eval(int(*fp)(std::string&) = nullptr) {
		switch (Op) {
		case(ADD): return LHS->eval(fp) + RHS->eval(fp);
		case(SUB): return LHS->eval(fp) - RHS->eval(fp);
		case(MUL): return LHS->eval(fp) * RHS->eval(fp);
		case(DIV): return LHS->eval(fp) / RHS->eval(fp);
		case(MOD): return LHS->eval(fp) % RHS->eval(fp);
		case(AND): return LHS->eval(fp) & RHS->eval(fp);
		case(OR):  return LHS->eval(fp) | RHS->eval(fp);
		case(XOR): return LHS->eval(fp) ^ RHS->eval(fp);
		case(LAND):return LHS->eval(fp) && RHS->eval(fp);
		case(LOR): return LHS->eval(fp) || RHS->eval(fp);
		case(SFTL):return LHS->eval(fp) << RHS->eval(fp);
		case(SFTR):return LHS->eval(fp) >> RHS->eval(fp);
		case(EQ):  return LHS->eval(fp) == RHS->eval(fp);
		case(NE):  return LHS->eval(fp) != RHS->eval(fp);
		case(LT):  return LHS->eval(fp) <  RHS->eval(fp);
		case(LE):  return LHS->eval(fp) <= RHS->eval(fp);
		case(GT):  return LHS->eval(fp) >  RHS->eval(fp);
		case(GE):  return LHS->eval(fp) >= RHS->eval(fp);
		}
		return 0;
	}
};

//-----------------------------------------------------------------------------
// ConditionalExprAST - Expression class for a conditinal operator.
class ConditionalExprAST : public ExprAST {
	std::unique_ptr<ExprAST> COND,LHS, RHS;
public:
	ConditionalExprAST(std::unique_ptr<ExprAST> COND, 
		std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
		: COND(std::move(COND)) , LHS(std::move(LHS)), RHS(std::move(RHS)) {}
	virtual int eval(int(*fp)(std::string&) = nullptr) {
		return COND->eval(fp) ? LHS->eval(fp) : RHS->eval(fp);
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
variable_expression (terminate)
: variable
*/
static std::unique_ptr<ExprAST> variable_expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	assert(tokens.front().type == VAR);
	auto Result = std::make_unique<VariableExprAST>(tokens.front().str);
	tokens.pop_front();											// eat variable
	return std::move(Result);
}



/*-----------------------------------------------------------------------------
unary_expression
*/
static std::unique_ptr<ExprAST> unary_expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	int op = tokens.front().type;
	if (op == ADD || op == SUB || op == INV || op == NOT) {
		tokens.pop_front();	//eat op
		auto RHS = primary_expression(tokens);
		if (!RHS) return nullptr;
		return std::make_unique<UnaryExprAST>(op, std::move(RHS));
	}
	return primary_expression(tokens);
}


/*-----------------------------------------------------------------------------
multiplicative_expression
*/
static std::unique_ptr<ExprAST> multiplicative_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = unary_expression(tokens);
	if (!LHS) return nullptr;
	int op = tokens.front().type;
	if (op == MUL || op == DIV || op == MOD) {
		tokens.pop_front();	//eat op
		auto RHS = unary_expression(tokens);
		if (!RHS) 		return nullptr;
		return multiplicative_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
additive_expression
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
shift_expression
*/
static std::unique_ptr<ExprAST> shift_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = additive_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == SFTL || op == SFTR) {
		tokens.pop_front();	//eat op
		auto RHS = additive_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return shift_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
relational_expression
*/
static std::unique_ptr<ExprAST> relational_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = shift_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == LT || op == LE || op == GT || op == GE) {
		tokens.pop_front();	//eat op
		auto RHS = shift_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return relational_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
equality_expression
*/
static std::unique_ptr<ExprAST> equality_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = relational_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == EQ || op == NE) {
		tokens.pop_front();	//eat op
		auto RHS = relational_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return equality_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
and_expression
*/
static std::unique_ptr<ExprAST> and_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = equality_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == AND) {
		tokens.pop_front();	//eat op
		auto RHS = equality_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return and_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
exclusive_or_expression
*/
static std::unique_ptr<ExprAST> exclusive_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = and_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == XOR) {
		tokens.pop_front();	//eat op
		auto RHS = and_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return exclusive_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
inclusive_or_expression
*/
static std::unique_ptr<ExprAST> inclusive_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = exclusive_or_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == XOR) {
		tokens.pop_front();	//eat op
		auto RHS = exclusive_or_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return inclusive_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
logical_and_expression
*/
static std::unique_ptr<ExprAST> logical_and_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = inclusive_or_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == LAND) {
		tokens.pop_front();	//eat op
		auto RHS = inclusive_or_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return logical_and_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}

/*-----------------------------------------------------------------------------
logical_or_expression
*/
static std::unique_ptr<ExprAST> logical_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> LHS) {
	FUNCTION_CALL_TRACE();
	if (!LHS) LHS = logical_and_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	int op = tokens.front().type;
	if (op == LOR) {
		tokens.pop_front();	//eat op
		auto RHS = logical_and_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return logical_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(LHS), std::move(RHS)));
	}
	return LHS;
}


/*-----------------------------------------------------------------------------
conditional_expression
*/
static std::unique_ptr<ExprAST> conditional_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> COND) {
	FUNCTION_CALL_TRACE();
	if(!COND) COND = logical_or_expression(tokens, nullptr);
	if (!COND) return nullptr;
	int op = tokens.front().type;
	if (op != QUESTION) {
		return COND;
	}

	tokens.pop_front();	//eat ?
	auto LHS = logical_or_expression(tokens, nullptr);
	if (!LHS) return nullptr;

	op = tokens.front().type;
	if (op == QUESTION) {
		return conditional_expression(tokens, std::move(LHS));
	}

	if (op == COLON) {
		tokens.pop_front();	//eat :
		auto RHS = logical_or_expression(tokens, nullptr);
		if (!RHS) 		return nullptr;
		return conditional_expression(tokens, std::make_unique<ConditionalExprAST>(std::move(COND), std::move(LHS), std::move(RHS)));
	}
	else {
		Error("expected ':'\n");
		//return nullptr;
	}
	return LHS;
}



/*-----------------------------------------------------------------------------
expression
: equality_expression
*/
static std::unique_ptr<ExprAST> expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	return conditional_expression(tokens, nullptr);
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
	case VAR:
		return variable_expression(tokens);
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
int eval(const std::string expr_str, int(*fp)(std::string&) = nullptr) {
	return expression(lexer(expr_str))->eval(fp);
}



//=============================================================================
//test & main
#include <gtest/gtest.h>

#define TO_STR(...) #__VA_ARGS__
#define TEST_EVAL(expr) ASSERT_EQ( (int)(expr), (int)eval(TO_STR(expr)))

TEST(eval, immidate)
{
	TEST_EVAL(3 + 2);
	TEST_EVAL(3 - 2);
	TEST_EVAL(3 * 2);
	TEST_EVAL(3 / 2);
	TEST_EVAL(3 % 2);
	TEST_EVAL(3 << 2);
	TEST_EVAL(3 >> 1);
	TEST_EVAL(3 <  2);
	TEST_EVAL(3 <= 2);
	TEST_EVAL(3 >  2);
	TEST_EVAL(3 >= 2);
	TEST_EVAL(3 == 2);
	TEST_EVAL(3 != 2);
	TEST_EVAL(3 & 2);
	TEST_EVAL(3 ^ 2);
	TEST_EVAL(3 | 2);
	TEST_EVAL(3 && 2);
	TEST_EVAL(3 || 2);
	TEST_EVAL(3 ? 2: 1);
	TEST_EVAL(+3);
	TEST_EVAL(-3);
	TEST_EVAL(~3);
	TEST_EVAL(!3);
	TEST_EVAL(1 + 2 * 3);
	TEST_EVAL(1 * 2 + 3);
	TEST_EVAL(1 * (2 + 3));
	TEST_EVAL((1 * 2) + 3);
	TEST_EVAL(1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	TEST_EVAL(1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9);
	TEST_EVAL(1 + 2 * 3 + 4 * 5 + 6 * 7 + 8 * 9);
	TEST_EVAL((1 + 2) * (3 + 4) * (5 + 6) * (7 + 8) * 9);
	TEST_EVAL((1 + 2 * 3) + (4 * 5 + 6) * (7 + 8 * 9));
	TEST_EVAL(((1 + 2 )* 3) + (4 * (5 + 6)) * ((7 + 8) * 9));
	TEST_EVAL((1 + 2 * 3) ? (4 * 5 + 6) : (7 + 8 * 9));
	TEST_EVAL(1 + 2 ? 3 + 4 : 5 + 6);
	TEST_EVAL(1 + 2 ? 3 + 4 ? 5 + 6 : 7 + 8 : 9);

}


std::map<std::string, int> SymbolMap;
int getVar(std::string& symbol) {
	return SymbolMap[symbol];
}

TEST(eval, variable)
{
	std::string symbol = "hoge";
	SymbolMap[symbol] = 2;
	ASSERT_EQ((int)SymbolMap[symbol], (int)eval(symbol, getVar));
}


int main(int argc, char** argv)
{
#if 0
	//	std::string line = "1223+123";
	//std::string line = "1223 + 123 * (3 + 3) && 1+3>>3";
	//std::string line = " 123+33*44";
//	std::string line = "1 * 2 + 3";
	std::string line = "1 + 2 ? 3 + 4 ? 5 + 6 : 7 + 8 : 9";

	std::list<Token> tokens = lexer(line);

	std::cout << line << std::endl;
	for (auto itr = tokens.begin(); itr != tokens.end(); itr++) {
//		std::cout << itr->type << ":" << itr->str << std::endl;
		std::cout << itr->str << std::endl;
	}
	auto expr = expression(tokens);
	std::cout << expr->eval() << std::endl;
#endif
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

