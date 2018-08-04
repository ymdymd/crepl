#include "macro.h"
#include "expr.h"
#include <assert.h>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string>

namespace expr {

//=============================================================================
// util
#ifdef _DEBUG
#define FUNCTION_CALL_TRACE(msg)                                               \
    std::cout << __FUNCTION__ << ":" << (msg) << std::endl
#else
#define FUNCTION_CALL_TRACE(msg)
#endif

//=============================================================================
// lexer

//-----------------------------------------------------------------------------
// Lexer
// lineの文字列をtokenに分割する。
std::list<Token> lexer(const std::string &line) {
    std::list<Token> tokens;
    auto itr = line.cbegin();
    auto ite = line.cend();

    std::vector<Token> keywords = {
        {IMMX, R"(^0[xX][0-9a-fA-F]+)"},
        {IMMB, R"(^0[bB][0-1]+)"},
        {IMM, R"(^[0-9]+)"},
        {VAR, R"(^[a-zA-Z][a-zA-Z0-9]*)"},
        {REG, R"(^\%[a-zA-Z][0-9]+)"},
        {ASSIGN_SL, R"(^\<\<\=)"},
        {ASSIGN_SR, R"(^\>\>\=)"},
        {ASSIGN_OR, R"(^\|\=)"},
        {ASSIGN_XOR, R"(^\^\=)"},
        {ASSIGN_AND, R"(^\&\=)"},
        {ASSIGN_ADD, R"(^\+\=)"},
        {ASSIGN_SUB, R"(^\-\=)"},
        {ASSIGN_MUL, R"(^\*\=)"},
        {ASSIGN_DIV, R"(^\/\=)"},
        {ASSIGN_MOD, R"(^\%\=)"},
        {SFTL, R"(^\<\<)"},
        {SFTR, R"(^\>\>)"},
        {EQ, R"(^\=\=)"},
        {NE, R"(^\!\=)"},
        {LE, R"(^\<\=)"},
        {GE, R"(^\>\=)"},
        {LAND, R"(^\&\&)"},
        {LOR, R"(^\|\|)"},
        {LT, R"(^\<)"},
        {GT, R"(^\>)"},
        {ADD, R"(^\+)"},
        {SUB, R"(^\-)"},
        {MUL, R"(^\*)"},
        {DIV, R"(^\/)"},
        {MOD, R"(^\%)"},
        {AND, R"(^\&)"},
        {OR, R"(^\|)"},
        {XOR, R"(^\^)"},
        {INV, R"(^\~)"},
        {NOT, R"(^\!)"},
        {PARL, R"(^\()"},
        {PARR, R"(^\))"},
        {SEMICOLON, R"(^\;)"},
        {COLON, R"(^\:)"},
        {QUESTION, R"(^\?)"},
        {ASSIGN, R"(^\=)"},
    };

    while (itr != ite) {
        std::smatch m;

        // skip white spcae
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
        } else { //見つからなかった場合は、残りをすべてtokensに入れる
#if 1
            throw expr_error("invalid token");
#else
            token.str = std::string(itr, ite);
            token.type = INVALID;
            tokens.push_back(token);
            break;
#endif
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
  public:
    const int Val;
    IntegerExprAST(int val) : ExprAST(IMM), Val(val) {}
    virtual int eval(int &(*fp)(const std::string &, void *) = nullptr,
                     void *_this = nullptr) {
        UNUSED(fp);
        UNUSED(_this);
        return Val;
    }
};

//-----------------------------------------------------------------------------
// VariableExprAST - Expression class for referencing a variable, like "a".
// VariableExprAST - "a"のような変数を参照するための式クラス。
class VariableExprAST : public ExprAST {
  public:
    const std::string Name;
    VariableExprAST(const std::string &Name) : ExprAST(VAR), Name(Name) {}
    virtual int eval(int &(*fp)(const std::string &, void *) = nullptr,
                     void *_this = nullptr) {
        return fp ? fp(Name, _this) : 0;
    }
};

//-----------------------------------------------------------------------------
// UnaryExprAST - Expression class for a unary operator.
class UnaryExprAST : public ExprAST {
    std::unique_ptr<ExprAST> rhs;

  public:
    UnaryExprAST(Type type, std::unique_ptr<ExprAST> rhs)
        : ExprAST(type), rhs(std::move(rhs)) {}
    virtual int eval(int &(*fp)(const std::string &, void *) = nullptr,
                     void *_this = nullptr) {
        switch (type) {
        case (ADD):
            return +rhs->eval(fp, _this);
        case (SUB):
            return -rhs->eval(fp, _this);
        case (INV):
            return ~rhs->eval(fp, _this);
        case (NOT):
            return !rhs->eval(fp, _this);
        default:
            throw expr_error("unknown operator");
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
    BinaryExprAST(Type type, std::unique_ptr<ExprAST> lhs,
                  std::unique_ptr<ExprAST> rhs)
        : ExprAST(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    virtual int eval(int &(*fp)(const std::string &, void *) = nullptr,
                     void *_this = nullptr) {
        switch (type) {
        case (ADD):
            return lhs->eval(fp, _this) + rhs->eval(fp, _this);
        case (SUB):
            return lhs->eval(fp, _this) - rhs->eval(fp, _this);
        case (MUL):
            return lhs->eval(fp, _this) * rhs->eval(fp, _this);
        case (DIV):
            return lhs->eval(fp, _this) / rhs->eval(fp, _this);
        case (MOD):
            return lhs->eval(fp, _this) % rhs->eval(fp, _this);
        case (AND):
            return lhs->eval(fp, _this) & rhs->eval(fp, _this);
        case (OR):
            return lhs->eval(fp, _this) | rhs->eval(fp, _this);
        case (XOR):
            return lhs->eval(fp, _this) ^ rhs->eval(fp, _this);
        case (LAND):
            return lhs->eval(fp, _this) && rhs->eval(fp, _this);
        case (LOR):
            return lhs->eval(fp, _this) || rhs->eval(fp, _this);
        case (SFTL):
            return lhs->eval(fp, _this) << rhs->eval(fp, _this);
        case (SFTR):
            return lhs->eval(fp, _this) >> rhs->eval(fp, _this);
        case (EQ):
            return lhs->eval(fp, _this) == rhs->eval(fp, _this);
        case (NE):
            return lhs->eval(fp, _this) != rhs->eval(fp, _this);
        case (LT):
            return lhs->eval(fp, _this) < rhs->eval(fp, _this);
        case (LE):
            return lhs->eval(fp, _this) <= rhs->eval(fp, _this);
        case (GT):
            return lhs->eval(fp, _this) > rhs->eval(fp, _this);
        case (GE):
            return lhs->eval(fp, _this) >= rhs->eval(fp, _this);
        default:
            throw expr_error("unknown operator");
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
                       std::unique_ptr<ExprAST> lhs,
                       std::unique_ptr<ExprAST> rhs)
        : ExprAST(QUESTION), cond(std::move(cond)), lhs(std::move(lhs)),
          rhs(std::move(rhs)) {}
    virtual int eval(int &(*fp)(const std::string &, void *) = nullptr,
                     void *_this = nullptr) {
        return cond->eval(fp, _this) ? lhs->eval(fp, _this)
                                     : rhs->eval(fp, _this);
    }
};

//-----------------------------------------------------------------------------
// AssignExprAST
class AssignExprAST : public ExprAST {
    std::unique_ptr<ExprAST> lhs, rhs;

  public:
    AssignExprAST(Type type, std::unique_ptr<ExprAST> lhs,
                  std::unique_ptr<ExprAST> rhs)
        : ExprAST(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    virtual int eval(int &(*fp)(const std::string &, void *) = nullptr,
                     void *_this = nullptr) {
        if (!fp)
            return 0;

        if (lhs->type != VAR) {
            throw expr_error("cannot assign to except for variables");
        }
        VariableExprAST *lhs_ast = static_cast<VariableExprAST *>(lhs.get());
        int &lhs_ref = fp(lhs_ast->Name, _this);
        switch (type) {
        case (ASSIGN):
            return lhs_ref = rhs->eval(fp, _this);
        case (ASSIGN_OR):
            return lhs_ref |= rhs->eval(fp, _this);
        case (ASSIGN_XOR):
            return lhs_ref ^= rhs->eval(fp, _this);
        case (ASSIGN_AND):
            return lhs_ref &= rhs->eval(fp, _this);
        case (ASSIGN_SL):
            return lhs_ref <<= rhs->eval(fp, _this);
        case (ASSIGN_SR):
            return lhs_ref >>= rhs->eval(fp, _this);
        case (ASSIGN_ADD):
            return lhs_ref += rhs->eval(fp, _this);
        case (ASSIGN_SUB):
            return lhs_ref -= rhs->eval(fp, _this);
        case (ASSIGN_MUL):
            return lhs_ref *= rhs->eval(fp, _this);
        case (ASSIGN_DIV):
            return lhs_ref /= rhs->eval(fp, _this);
        case (ASSIGN_MOD):
            return lhs_ref %= rhs->eval(fp, _this);
        default:
            throw expr_error("unknown operator");
        }
        return 0;
    }
};

//=============================================================================
// Parser

static std::unique_ptr<ExprAST> primary_expression(std::list<Token> &tokens);
static std::unique_ptr<ExprAST> expression(std::list<Token> &tokens);

/*-----------------------------------------------------------------------------
unary_expression
*/
static std::unique_ptr<ExprAST> unary_expression(std::list<Token> &tokens) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    Type op = tokens.front().type;
    if (op == ADD || op == SUB || op == INV || op == NOT) {
        tokens.pop_front(); // eat op
        auto rhs = primary_expression(tokens);
        assert(rhs);
        return std::make_unique<UnaryExprAST>(op, std::move(rhs));
    }
    return primary_expression(tokens);
}

#if 0
/*-----------------------------------------------------------------------------
multiplicative_expression
*/
static std::unique_ptr<ExprAST> multiplicative_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = unary_expression(tokens);
	assert(lhs);
	Type op = tokens.front().type;
	if (op == MUL || op == DIV || op == MOD) {
		tokens.pop_front();	//eat op
		auto rhs = unary_expression(tokens);
		assert(rhs);
		return multiplicative_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
additive_expression
*/
static std::unique_ptr<ExprAST> additive_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = multiplicative_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == ADD || op == SUB) {
		tokens.pop_front();	//eat op
		auto rhs = multiplicative_expression(tokens, nullptr);
		assert(rhs);
		return additive_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
shift_expression
*/
static std::unique_ptr<ExprAST> shift_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = additive_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == SFTL || op == SFTR) {
		tokens.pop_front();	//eat op
		auto rhs = additive_expression(tokens, nullptr);
		assert(rhs);
		return shift_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
relational_expression
*/
static std::unique_ptr<ExprAST> relational_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = shift_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == LT || op == LE || op == GT || op == GE) {
		tokens.pop_front();	//eat op
		auto rhs = shift_expression(tokens, nullptr);
		assert(rhs);
		return relational_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
equality_expression
*/
static std::unique_ptr<ExprAST> equality_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = relational_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == EQ || op == NE) {
		tokens.pop_front();	//eat op
		auto rhs = relational_expression(tokens, nullptr);
		assert(rhs);
		return equality_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
and_expression
*/
static std::unique_ptr<ExprAST> and_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = equality_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == AND) {
		tokens.pop_front();	//eat op
		auto rhs = equality_expression(tokens, nullptr);
		assert(rhs);
		return and_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
exclusive_or_expression
*/
static std::unique_ptr<ExprAST> exclusive_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = and_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == XOR) {
		tokens.pop_front();	//eat op
		auto rhs = and_expression(tokens, nullptr);
		assert(rhs);
		return exclusive_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
inclusive_or_expression
*/
static std::unique_ptr<ExprAST> inclusive_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = exclusive_or_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == XOR) {
		tokens.pop_front();	//eat op
		auto rhs = exclusive_or_expression(tokens, nullptr);
		assert(rhs);
		return inclusive_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
logical_and_expression
*/
static std::unique_ptr<ExprAST> logical_and_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = inclusive_or_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == LAND) {
		tokens.pop_front();	//eat op
		auto rhs = inclusive_or_expression(tokens, nullptr);
		assert(rhs);
		return logical_and_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
logical_or_expression
*/
static std::unique_ptr<ExprAST> logical_or_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	if (!lhs) lhs = logical_and_expression(tokens, nullptr);
	assert(lhs);

	Type op = tokens.front().type;
	if (op == LOR) {
		tokens.pop_front();	//eat op
		auto rhs = logical_and_expression(tokens, nullptr);
		assert(rhs);
		return logical_or_expression(tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs)));
	}
	return lhs;
}

/*-----------------------------------------------------------------------------
binary_expression
*/
static std::unique_ptr<ExprAST> binary_expression(std::list<Token>& tokens, std::unique_ptr<ExprAST> lhs) {
	FUNCTION_CALL_TRACE(tokens.front().str);
	return logical_or_expression(tokens, std::move(lhs));
}
#else
/*-----------------------------------------------------------------------------
binary_expression
*/
static std::unique_ptr<ExprAST>
binary_expression(std::list<Token> &tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    if (!lhs)
        lhs = unary_expression(tokens);
    assert(lhs);
    while (1) {
        Type type = tokens.front().type;
        // 現在のトークンが、2項演算子でない場合は、lhsを返す。
        if (type < BINOP_BIGIN || BINOP_END < type)
            return lhs;

        tokens.pop_front(); // eat op
        auto rhs = unary_expression(tokens);
        assert(rhs);

        //現在の演算優先度が、rhsの後の二項演算の優先度より低い場合は、
        //現在のrhsを初期ノードとした、二項演算ツリーを作る。
        if (type < tokens.front().type) {
            rhs = binary_expression(tokens, std::move(rhs));
            assert(rhs);
        }

        // merge lhs/rhs
        lhs = std::make_unique<BinaryExprAST>(type, std::move(lhs),
                                              std::move(rhs));
    }
}
#endif

/*-----------------------------------------------------------------------------
conditional_expression
<conditional-expression> ::= <logical-or-expression>
                           | <logical-or-expression> ? <expression> :
<conditional-expression>
<conditional-expression> ::= <binary_expression>
                           | <binary_expression> ? <expression> :
<conditional-expression>

*/
static std::unique_ptr<ExprAST>
conditional_expression(std::list<Token> &tokens,
                       std::unique_ptr<ExprAST> cond) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    if (!cond)
        cond = binary_expression(tokens, nullptr);
    assert(cond);
    Type op = tokens.front().type;
    if (op != QUESTION) {
        return cond;
    }

    tokens.pop_front(); // eat ?
    auto lhs = expression(tokens);
    assert(lhs);

    op = tokens.front().type;
    if (op == COLON) {
        tokens.pop_front(); // eat :
        auto rhs = conditional_expression(tokens, nullptr);
        assert(rhs);
        return conditional_expression(
            tokens, std::make_unique<ConditionalExprAST>(
                        std::move(cond), std::move(lhs), std::move(rhs)));
    } else {
        throw expr_error("expected ':'\n");
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
assignment_expression
        : unary_expression assignment_operator assignment_expression
        | conditional_expression
*/
static std::unique_ptr<ExprAST>
assignment_expression(std::list<Token> &tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens.front().str);

    if (!lhs)
        lhs = conditional_expression(tokens, nullptr);
    assert(lhs);

    Type opc = tokens.front().type;
    if (opc < ASSIGN_BIGIN || ASSIGN_END < opc)
        return lhs;

    tokens.pop_front(); // eat opc
    auto rhs = assignment_expression(tokens, nullptr);
    assert(rhs);
    return assignment_expression(
        tokens,
        std::make_unique<AssignExprAST>(opc, std::move(lhs), std::move(rhs)));
}

/*-----------------------------------------------------------------------------
expression
: equality_expression
*/
static std::unique_ptr<ExprAST> expression(std::list<Token> &tokens) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    //	return conditional_expression(tokens, nullptr);
    return assignment_expression(tokens, nullptr);
}

/*-----------------------------------------------------------------------------
integer_expression (terminate)
: number
*/
static std::unique_ptr<ExprAST> integer_expression(std::list<Token> &tokens) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    int value = 0;
    if (tokens.front().type == IMM)
        value = (int)std::stoi(tokens.front().str, nullptr, 0);
    else if (tokens.front().type == IMMX)
        value = (int)std::stoi(tokens.front().str.substr(2), nullptr, 16);
    else if (tokens.front().type == IMMB)
        value = (int)std::stoi(tokens.front().str.substr(2), nullptr, 2);
    else
        assert(0 && "illigal token type");
    auto Result = std::make_unique<IntegerExprAST>(value);
    tokens.pop_front(); // consume the number
    return std::move(Result);
}

/*-----------------------------------------------------------------------------
variable_expression (terminate)
: variable
*/
static std::unique_ptr<ExprAST> variable_expression(std::list<Token> &tokens) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    assert(tokens.front().type == VAR || tokens.front().type == REG);
    auto Result = std::make_unique<VariableExprAST>(tokens.front().str);
    tokens.pop_front(); // eat variable
    return std::move(Result);
}

/*-----------------------------------------------------------------------------
primary_expression
: integer_expression　(terminate)
| PARL expression PARR
*/
static std::unique_ptr<ExprAST> primary_expression(std::list<Token> &tokens) {
    FUNCTION_CALL_TRACE(tokens.front().str);
    switch (tokens.front().type) {
    default:
        throw expr_error("unknown token when expecting an expression");
    case IMM:
    case IMMX:
    case IMMB:
        return integer_expression(tokens);
    case VAR:
    case REG:
        return variable_expression(tokens);
    case PARL: {
        tokens.pop_front();          // eat (.
        auto V = expression(tokens); // expression
        assert(V);
        //副次式を解析した後、”)”の出現がない可能性がある。
        if (tokens.front().type != PARR) {
            throw expr_error("expected ')'");
        }
        tokens.pop_front(); // eat ).
        return V;
    }
    }
}

//=============================================================================
std::unique_ptr<ExprAST> parser(std::list<Token> &tokens) {
    auto V = expression(tokens);
    if (tokens.front().type == EOL)
        return V;

    if (tokens.front().type == PARR)
        throw expr_error("expected '('");
    else
        throw expr_error("unknown token when expecting an expression '" +
                         tokens.front().str + "'");
    // unreachable
    return nullptr;
}

//=============================================================================
// evalute expr_str
std::unique_ptr<ExprAST> parser(const std::string &expr_str) {
    auto tokens = lexer(expr_str);
    return parser(tokens);
}

//=============================================================================
// evalute expr_str
int eval(const std::string expr_str, int &(*fp)(const std::string &, void *),
         void *_this) {
    return parser(expr_str)->eval(fp, _this);
}

} // namespace
