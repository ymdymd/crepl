#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <memory>
#include <map>
#include <assert.h>
#include "expr.h"

namespace expr {


//=============================================================================
// util
#ifdef _DEBUG
#define FUNCTION_CALL_TRACE() std::cout << __FUNCTION__ << std::endl
#else
#define FUNCTION_CALL_TRACE() 
#endif

//-----------------------------------------------------------------------------
// error
static void Error(const std::string& msg) {
	std::cerr << msg;
}


//=============================================================================
// lexer




//-----------------------------------------------------------------------------
// Lexer
// lineの文字列をtokenに分割する。
std::list<Token> lexer(std::string line) {
	std::list<Token> tokens;
	auto itr = line.cbegin();
	auto ite = line.cend();

	std::vector<Token> keywords = {
		{ IMM,		R"(^[0-9]+)" },
		{ VAR,		R"(^[a-zA-Z][a-zA-Z0-9]+)" },
		{ REG,		R"(^\%[a-zA-Z][0-9]+)" },
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
	};

	while (itr != ite) {
		std::smatch m;

		//skip white spcae
		if (regex_search(itr, ite, m, std::regex(R"(^[ \t]+)"))) {
			itr = m[0].second;
			continue;
		}

		Token token;
		size_t i;
		for (i = 0; i < keywords.size(); i++) {
			if (regex_search(itr, ite, m, std::regex(keywords[i].str))) {
				itr = m[0].second;
				token.str = m[0];
				token.type = keywords[i].type;
				break;
			}
		}

		if (token.type) {
			tokens.push_back(token);
		}
		else {	//見つからなかった場合は、残りをすべてtokensに入れる
			token.str = std::string(itr, ite);
			token.type = keywords[i].type;
			tokens.push_back(token);
			break;
		}
	}

	tokens.push_back(Token(EOL, std::string("")));
	return tokens;
}


//=============================================================================
// AST (Abstract Syntax Tree)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// IntegerExprAST - Expression class for integer literals like "1".
// IntegerExprAST - "1"のような整数数値リテラルのための式クラス。
class IntegerExprAST : public ExprAST {
	int Val;
public:
	IntegerExprAST(int val) : ExprAST(IMM) , Val(val) {}
	virtual int eval(int(*fp)(std::string&, void*) = nullptr, void* _this = nullptr) { return Val; }
};

//-----------------------------------------------------------------------------
// VariableExprAST - Expression class for referencing a variable, like "a".
// VariableExprAST - "a"のような変数を参照するための式クラス。
class VariableExprAST : public ExprAST {
	std::string Name;
public:
	VariableExprAST(const std::string &Name) : ExprAST(VAR) , Name(Name) {}
	virtual int eval(int(*fp)(std::string&, void*) = nullptr, void* _this = nullptr) { return fp ? fp(Name,_this) : 0; }
};

//-----------------------------------------------------------------------------
// UnaryExprAST - Expression class for a unary operator.
class UnaryExprAST : public ExprAST {
	std::unique_ptr<ExprAST> rhs;
public:
	UnaryExprAST(Type type, std::unique_ptr<ExprAST> rhs)
		: ExprAST(type), rhs(std::move(rhs)) {}
	virtual int eval(int(*fp)(std::string&, void*) = nullptr, void* _this = nullptr) {
		switch (type) {
		case(ADD): return +rhs->eval(fp, _this);
		case(SUB): return -rhs->eval(fp, _this);
		case(INV): return ~rhs->eval(fp, _this);
		case(NOT): return !rhs->eval(fp, _this);
		}
		return 0;
	}
};

//-----------------------------------------------------------------------------
// BinaryExprAST - Expression class for a binary operator.
// BinaryExprAST - 二項演算子のための式クラス。
class BinaryExprAST : public ExprAST {
	std::unique_ptr<ExprAST> lhs, rhs;
public:
	BinaryExprAST(Type type, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
		: ExprAST(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	virtual int eval(int(*fp)(std::string&, void*) = nullptr, void* _this = nullptr) {
		switch (type) {
		case(ADD): return lhs->eval(fp, _this) + rhs->eval(fp, _this);
		case(SUB): return lhs->eval(fp, _this) - rhs->eval(fp, _this);
		case(MUL): return lhs->eval(fp, _this) * rhs->eval(fp, _this);
		case(DIV): return lhs->eval(fp, _this) / rhs->eval(fp, _this);
		case(MOD): return lhs->eval(fp, _this) % rhs->eval(fp, _this);
		case(AND): return lhs->eval(fp, _this) & rhs->eval(fp, _this);
		case(OR):  return lhs->eval(fp, _this) | rhs->eval(fp, _this);
		case(XOR): return lhs->eval(fp, _this) ^ rhs->eval(fp, _this);
		case(LAND):return lhs->eval(fp, _this) && rhs->eval(fp, _this);
		case(LOR): return lhs->eval(fp, _this) || rhs->eval(fp, _this);
		case(SFTL):return lhs->eval(fp, _this) << rhs->eval(fp, _this);
		case(SFTR):return lhs->eval(fp, _this) >> rhs->eval(fp, _this);
		case(EQ):  return lhs->eval(fp, _this) == rhs->eval(fp, _this);
		case(NE):  return lhs->eval(fp, _this) != rhs->eval(fp, _this);
		case(LT):  return lhs->eval(fp, _this) <  rhs->eval(fp, _this);
		case(LE):  return lhs->eval(fp, _this) <= rhs->eval(fp, _this);
		case(GT):  return lhs->eval(fp, _this) >  rhs->eval(fp, _this);
		case(GE):  return lhs->eval(fp, _this) >= rhs->eval(fp, _this);
		}
		return 0;
	}
};

//-----------------------------------------------------------------------------
// ConditionalExprAST - Expression class for a conditinal operator.
class ConditionalExprAST : public ExprAST {
	std::unique_ptr<ExprAST> cond, lhs, rhs;
public:
	ConditionalExprAST(std::unique_ptr<ExprAST> cond,
		std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
		: ExprAST(QUESTION) , cond(std::move(cond)), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
	virtual int eval(int(*fp)(std::string&, void*) = nullptr, void* _this = nullptr) {
		return cond->eval(fp, _this) ? lhs->eval(fp, _this) : rhs->eval(fp, _this);
	}
};




//=============================================================================
// Parser

static std::unique_ptr<ExprAST> primary_expression(std::list<Token>& tokens);




/*-----------------------------------------------------------------------------
unary_expression
*/
static std::unique_ptr<ExprAST> unary_expression(std::list<Token>& tokens) {
	FUNCTION_CALL_TRACE();
	Type op = tokens.front().type;
	if (op == ADD || op == SUB || op == INV || op == NOT) {
		tokens.pop_front();	//eat op
		auto rhs = primary_expression(tokens);
		if (!rhs) return nullptr;
		return std::make_unique<UnaryExprAST>(op, std::move(rhs));
	}
	return primary_expression(tokens);
}


#if 0
/*-----------------------------------------------------------------------------
multiplicative_expression
*/
static std::unique_ptr<ExprAST> multiplicative_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = unary_expression(tokens);
	if (!lhs) return nullptr;
	Type op = tokens.front().type;
	if (op == MUL || op == DIV || op == MOD) {
		tokens.pop_front();	//eat op
		auto rhs = unary_expression(tokens);
		if (!rhs) 		return nullptr;
		return multiplicative_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
additive_expression
*/
static std::unique_ptr<ExprAST> additive_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = multiplicative_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == ADD || op == SUB) {
		tokens.pop_front();	//eat op
		auto rhs = multiplicative_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return additive_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
shift_expression
*/
static std::unique_ptr<ExprAST> shift_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = additive_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == SFTL || op == SFTR) {
		tokens.pop_front();	//eat op
		auto rhs = additive_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return shift_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
relational_expression
*/
static std::unique_ptr<ExprAST> relational_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = shift_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == LT || op == LE || op == GT || op == GE) {
		tokens.pop_front();	//eat op
		auto rhs = shift_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return relational_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
equality_expression
*/
static std::unique_ptr<ExprAST> equality_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = relational_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == EQ || op == NE) {
		tokens.pop_front();	//eat op
		auto rhs = relational_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return equality_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
and_expression
*/
static std::unique_ptr<ExprAST> and_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = equality_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == AND) {
		tokens.pop_front();	//eat op
		auto rhs = equality_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return and_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
exclusive_or_expression
*/
static std::unique_ptr<ExprAST> exclusive_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = and_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == XOR) {
		tokens.pop_front();	//eat op
		auto rhs = and_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return exclusive_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
inclusive_or_expression
*/
static std::unique_ptr<ExprAST> inclusive_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = exclusive_or_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == XOR) {
		tokens.pop_front();	//eat op
		auto rhs = exclusive_or_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return inclusive_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
logical_and_expression
*/
static std::unique_ptr<ExprAST> logical_and_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = inclusive_or_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == LAND) {
		tokens.pop_front();	//eat op
		auto rhs = inclusive_or_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return logical_and_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
logical_or_expression
*/
static std::unique_ptr<ExprAST> logical_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = logical_and_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	Type op = tokens.front().type;
	if (op == LOR) {
		tokens.pop_front();	//eat op
		auto rhs = logical_and_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return logical_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
binary_expression
*/
static std::unique_ptr<ExprAST> binary_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	return logical_or_expression(tokens, std::move(lhs));
}
#else
/*-----------------------------------------------------------------------------
binary_expression
*/
static std::unique_ptr<ExprAST> binary_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE();
	if (!lhs) lhs = unary_expression(tokens);
	if (!lhs) return nullptr;
	while (1) {
		Type type = tokens.front().type;
		// 現在のトークンが、2項演算子でない場合は、lhsを返す。
		if (type < BINOP_BIGIN || BINOP_END < type) 	return lhs;

		tokens.pop_front();			//eat op
		auto rhs = unary_expression(tokens);
		if (!rhs) 		return nullptr;

		//現在の演算優先度が、rhsの後の二項演算の優先度より低い場合は、
		//現在のrhsを初期ノードとした、二項演算ツリーを作る。
		if (type < tokens.front().type) {
			rhs = binary_expression(tokens, std::move(rhs));
			if (!rhs) 		return nullptr;
		}

		//merge lhs/rhs
		lhs = std::make_unique<BinaryExprAST>(type, std::move(lhs), std::move(rhs));
	}
}
#endif

/*-----------------------------------------------------------------------------
conditional_expression
*/
static std::unique_ptr<ExprAST> conditional_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> cond) {
	FUNCTION_CALL_TRACE();
	if (!cond) cond = binary_expression(tokens, nullptr);
	if (!cond) return nullptr;
	Type op = tokens.front().type;
	if (op != QUESTION) {
		return cond;
	}

	tokens.pop_front();	//eat ?
	auto lhs = binary_expression(tokens, nullptr);
	if (!lhs) return nullptr;

	op = tokens.front().type;
	if (op == QUESTION) {
		return conditional_expression(tokens, std::move(lhs));
	}

	if (op == COLON) {
		tokens.pop_front();	//eat :
		auto rhs = binary_expression(tokens, nullptr);
		if (!rhs) 		return nullptr;
		return conditional_expression(tokens, std::make_unique<ConditionalExprAST>(std::move(cond), std::move(lhs), std::move(rhs)));
	}
	else {
		Error("expected ':'\n");
		//return nullptr;
	}
	return lhs;
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
	assert(tokens.front().type == VAR || tokens.front().type == REG);
	auto Result = std::make_unique<VariableExprAST>(tokens.front().str);
	tokens.pop_front();											// eat variable
	return std::move(Result);
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
	case REG:
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
std::unique_ptr<ExprAST> parser(const std::string expr_str) {
	return expression(lexer(expr_str));
}

//=============================================================================
// evalute expr_str
int eval(const std::string expr_str, int(*fp)(std::string&, void*), void* _this) {
	return parser(expr_str)->eval(fp, _this);
}


}	//namespace


