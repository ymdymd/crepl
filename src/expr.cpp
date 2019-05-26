#include "expr.h"
#include "macro.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <utility>

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

    if (token.type != EOL) {
      tokens.push_back(token);
    } else { //見つからなかった場合は、残りをすべてtokensに入れる
#if 1
      throw expr_error("invalid token");
#else
      token.str = std::string(itr, ite);
      token.type = INVALID;
      tokens->push_back(token);
      break;
#endif
    }
  }

  tokens.emplace_back(EOL, std::string(""));
  return tokens;
}

// Forward declaration
class IntegerExprAST;
class VariableExprAST;
class UnaryExprAST;
class BinaryExprAST;
class ConditionalExprAST;
class AssignExprAST;

//=============================================================================
//! \brief abstruct visitor class for expression AST
class ExprVisitor {
public:
  ExprVisitor() = default;
  virtual ~ExprVisitor() = default;

  virtual void visit(const IntegerExprAST &) = 0;
  virtual void visit(const VariableExprAST &) = 0;
  virtual void visit(const UnaryExprAST &) = 0;
  virtual void visit(const BinaryExprAST &) = 0;
  virtual void visit(const ConditionalExprAST &) = 0;
  virtual void visit(const AssignExprAST &) = 0;
};

//=============================================================================
// AST (Abstract Syntax Tree)
//-----------------------------------------------------------------------------
//! \brief Expression class for integer literals like "1".
//! \brief "1"のような整数数値リテラルのための式クラス。
class IntegerExprAST : public ExprAST {
public:
  const int Val;
  explicit IntegerExprAST(int val) : ExprAST(IMM), Val(val) {}
  void accept(ExprVisitor *visitor) override { visitor->visit(*this); }
};

//-----------------------------------------------------------------------------
//! \brief Expression class for referencing a variable, like "a".
//! \brief "a"のような変数を参照するための式クラス。
class VariableExprAST : public ExprAST {
public:
  const std::string Name;
  explicit VariableExprAST(std::string Name)
      : ExprAST(VAR), Name(std::move(Name)) {}
  void accept(ExprVisitor *visitor) override { visitor->visit(*this); }
};

//-----------------------------------------------------------------------------
//! \brief Expression class for a unary operator.
class UnaryExprAST : public ExprAST {
public:
  std::unique_ptr<ExprAST> rhs;

public:
  UnaryExprAST(Type type, std::unique_ptr<ExprAST> rhs)
      : ExprAST(type), rhs(std::move(rhs)) {}
  void accept(ExprVisitor *visitor) override { visitor->visit(*this); }
};

//-----------------------------------------------------------------------------
//! \brief Expression class for a binary operator.
//! \brief 二項演算子のための式クラス。
class BinaryExprAST : public ExprAST {
public:
  const std::unique_ptr<ExprAST> lhs, rhs;

public:
  BinaryExprAST(Type type, std::unique_ptr<ExprAST> lhs,
                std::unique_ptr<ExprAST> rhs)
      : ExprAST(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  void accept(ExprVisitor *visitor) override { visitor->visit(*this); }
};

//-----------------------------------------------------------------------------
//! \brief Expression class for a conditinal operator.
class ConditionalExprAST : public ExprAST {
public:
  const std::unique_ptr<ExprAST> cond, lhs, rhs;

public:
  ConditionalExprAST(std::unique_ptr<ExprAST> cond,
                     std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
      : ExprAST(QUESTION), cond(std::move(cond)), lhs(std::move(lhs)),
        rhs(std::move(rhs)) {}
  void accept(ExprVisitor *visitor) override { visitor->visit(*this); }
};

//-----------------------------------------------------------------------------
//! \brief Expression class for a assignment.
class AssignExprAST : public ExprAST {
public:
  const std::unique_ptr<ExprAST> lhs, rhs;

public:
  AssignExprAST(Type type, std::unique_ptr<ExprAST> lhs,
                std::unique_ptr<ExprAST> rhs)
      : ExprAST(type), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
  void accept(ExprVisitor *visitor) override { visitor->visit(*this); }
};

//=============================================================================
// Parser

static std::unique_ptr<ExprAST> primary_expression(std::list<Token> *tokens);
static std::unique_ptr<ExprAST> expression(std::list<Token> *tokens);

/*!----------------------------------------------------------------------------
\brief create unary_expression AST from tokens
 */
static std::unique_ptr<ExprAST> unary_expression(std::list<Token> *tokens) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  Type op = tokens->front().type;
  if (op == ADD || op == SUB || op == INV || op == NOT) {
    tokens->pop_front(); // eat op
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
static std::unique_ptr<ExprAST>
multiplicative_expression(std::list<Token> *tokens,
                          std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = unary_expression(tokens);
    assert(lhs);
    Type op = tokens->front().type;
    if (op == MUL || op == DIV || op == MOD) {
        tokens->pop_front(); // eat op
        auto rhs = unary_expression(tokens);
        assert(rhs);
        return multiplicative_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
additive_expression
*/
static std::unique_ptr<ExprAST>
additive_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = multiplicative_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == ADD || op == SUB) {
        tokens->pop_front(); // eat op
        auto rhs = multiplicative_expression(tokens, nullptr);
        assert(rhs);
        return additive_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
shift_expression
*/
static std::unique_ptr<ExprAST> shift_expression(std::list<Token> *tokens,
                                                 std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = additive_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == SFTL || op == SFTR) {
        tokens->pop_front(); // eat op
        auto rhs = additive_expression(tokens, nullptr);
        assert(rhs);
        return shift_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
relational_expression
*/
static std::unique_ptr<ExprAST>
relational_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = shift_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == LT || op == LE || op == GT || op == GE) {
        tokens->pop_front(); // eat op
        auto rhs = shift_expression(tokens, nullptr);
        assert(rhs);
        return relational_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
equality_expression
*/
static std::unique_ptr<ExprAST>
equality_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = relational_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == EQ || op == NE) {
        tokens->pop_front(); // eat op
        auto rhs = relational_expression(tokens, nullptr);
        assert(rhs);
        return equality_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
and_expression
*/
static std::unique_ptr<ExprAST> and_expression(std::list<Token> *tokens,
                                               std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = equality_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == AND) {
        tokens->pop_front(); // eat op
        auto rhs = equality_expression(tokens, nullptr);
        assert(rhs);
        return and_expression(tokens, std::make_unique<BinaryExprAST>(
                                          op, std::move(lhs), std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
exclusive_or_expression
*/
static std::unique_ptr<ExprAST>
exclusive_or_expression(std::list<Token> *tokens,
                        std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = and_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == XOR) {
        tokens->pop_front(); // eat op
        auto rhs = and_expression(tokens, nullptr);
        assert(rhs);
        return exclusive_or_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
inclusive_or_expression
*/
static std::unique_ptr<ExprAST>
inclusive_or_expression(std::list<Token> *tokens,
                        std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = exclusive_or_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == XOR) {
        tokens->pop_front(); // eat op
        auto rhs = exclusive_or_expression(tokens, nullptr);
        assert(rhs);
        return inclusive_or_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
logical_and_expression
*/
static std::unique_ptr<ExprAST>
logical_and_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = inclusive_or_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == LAND) {
        tokens->pop_front(); // eat op
        auto rhs = inclusive_or_expression(tokens, nullptr);
        assert(rhs);
        return logical_and_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
logical_or_expression
*/
static std::unique_ptr<ExprAST>
logical_or_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    if (!lhs)
        lhs = logical_and_expression(tokens, nullptr);
    assert(lhs);

    Type op = tokens->front().type;
    if (op == LOR) {
        tokens->pop_front(); // eat op
        auto rhs = logical_and_expression(tokens, nullptr);
        assert(rhs);
        return logical_or_expression(
            tokens, std::make_unique<BinaryExprAST>(op, std::move(lhs),
                                                    std::move(rhs)));
    }
    return lhs;
}

/*-----------------------------------------------------------------------------
binary_expression
*/
static std::unique_ptr<ExprAST>
binary_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
    FUNCTION_CALL_TRACE(tokens->front().str);
    return logical_or_expression(tokens, std::move(lhs));
}
#else
/*!----------------------------------------------------------------------------
\brief create unary expression AST from tokens

<binary_expression>
    ::= <unary_expression>
    | <binary_expression> binop <binary_expression>

*/
static std::unique_ptr<ExprAST>
binary_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  if (!lhs) {
    lhs = unary_expression(tokens);
  }
  assert(lhs);
  while (true) {
    Type type = tokens->front().type;
    // 現在のトークンが、2項演算子でない場合は、lhsを返す。
    if (type < BINOP_BIGIN || BINOP_END < type) {
      return lhs;
    }

    tokens->pop_front(); // eat op
    auto rhs = unary_expression(tokens);
    assert(rhs);

    //現在の演算優先度が、rhsの後の二項演算の優先度より低い場合は、
    //現在のrhsを初期ノードとした、二項演算ツリーを作る。
    if (type < tokens->front().type) {
      rhs = binary_expression(tokens, std::move(rhs));
      assert(rhs);
    }

    // merge lhs/rhs
    lhs = std::make_unique<BinaryExprAST>(type, std::move(lhs), std::move(rhs));
  }
}
#endif

/*!----------------------------------------------------------------------------
\brief create unary expression AST from tokens

<conditional-expression>
    ::= <binary_expression>
    | <binary_expression> ? <expression> : <conditional-expression>
 */
static std::unique_ptr<ExprAST>
conditional_expression(std::list<Token> *tokens,
                       std::unique_ptr<ExprAST> cond) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  if (!cond) {
    cond = binary_expression(tokens, nullptr);
  }
  assert(cond);
  Type op = tokens->front().type;
  if (op != QUESTION) {
    return cond;
  }

  tokens->pop_front(); // eat ?
  auto lhs = expression(tokens);
  assert(lhs);

  op = tokens->front().type;
  if (op == COLON) {
    tokens->pop_front(); // eat :
    auto rhs = conditional_expression(tokens, nullptr);
    assert(rhs);
    return conditional_expression(
        tokens, std::make_unique<ConditionalExprAST>(
                    std::move(cond), std::move(lhs), std::move(rhs)));
  }
  throw expr_error("expected ':'\n");

  return lhs;
} // namespace expr

/*!----------------------------------------------------------------------------
\brief create assignment expression AST from tokens

<assignment_expression>
    ::= <unary_expression> <assignment_operator> <assignment_expression>
    | <conditional_expression>
*/
static std::unique_ptr<ExprAST>
assignment_expression(std::list<Token> *tokens, std::unique_ptr<ExprAST> lhs) {
  FUNCTION_CALL_TRACE(tokens->front().str);

  if (!lhs) {
    lhs = conditional_expression(tokens, nullptr);
  }
  assert(lhs);

  Type opc = tokens->front().type;
  if (opc < ASSIGN_BIGIN || ASSIGN_END < opc) {
    return lhs;
  }

  tokens->pop_front(); // eat opc
  auto rhs = assignment_expression(tokens, nullptr);
  assert(rhs);
  return assignment_expression(
      tokens,
      std::make_unique<AssignExprAST>(opc, std::move(lhs), std::move(rhs)));
}

/*!----------------------------------------------------------------------------
\brief create expression AST from tokens

<expression> ::= <equality_expression>
*/
static std::unique_ptr<ExprAST> expression(std::list<Token> *tokens) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  return assignment_expression(tokens, nullptr);
}

/*!----------------------------------------------------------------------------
\brief create integer_expression(terminate) from tokens

<integer_expression> ::= <number>
*/
static std::unique_ptr<ExprAST> integer_expression(std::list<Token> *tokens) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  int value = 0;
  if (tokens->front().type == IMM) {
    value = std::stoi(tokens->front().str, nullptr, 0);
  } else if (tokens->front().type == IMMX) {
    value = static_cast<int>(
        std::stoul(tokens->front().str.substr(2), nullptr, 16));
  } else if (tokens->front().type == IMMB) {
    value =
        static_cast<int>(std::stoul(tokens->front().str.substr(2), nullptr, 2));
  } else {
    assert(0 && "illigal token type");
  }
  auto Result = std::make_unique<IntegerExprAST>(value);
  tokens->pop_front(); // consume the number
  return std::move(Result);
}

/*!----------------------------------------------------------------------------
\brief create variable_expression(terminate) from tokens

<variable_expression> ::= <variable>
*/
static std::unique_ptr<ExprAST> variable_expression(std::list<Token> *tokens) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  assert(tokens->front().type == VAR || tokens->front().type == REG);
  auto Result = std::make_unique<VariableExprAST>(tokens->front().str);
  tokens->pop_front(); // eat variable
  return std::move(Result);
}

/*!----------------------------------------------------------------------------
\brief primary_expression from tokens

<primary_expression>
    ::= <integer_expression>　(terminate)
    | ( <expression> )
*/
static std::unique_ptr<ExprAST> primary_expression(std::list<Token> *tokens) {
  FUNCTION_CALL_TRACE(tokens->front().str);
  switch (tokens->front().type) {
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
    tokens->pop_front();         // eat (.
    auto V = expression(tokens); // expression
    assert(V);
    //副次式を解析した後、”)”の出現がない可能性がある。
    if (tokens->front().type != PARR) {
      throw expr_error("expected ')'");
    }
    tokens->pop_front(); // eat ).
    return V;
  }
  }
}

//=============================================================================
std::unique_ptr<ExprAST> parser(std::list<Token> *tokens) {
  auto V = expression(tokens);
  if (tokens->front().type == EOL) {
    return V;
  }

  if (tokens->front().type == PARR) {
    throw expr_error("expected '('");
  }
  throw expr_error("unknown token when expecting an operator '" +
                   tokens->front().str + "'");

  // unreachable
  return nullptr;
}

//! \brief visitor to evalute expression AST
class EvalExprVisitor : public ExprVisitor {
public:
  int val{};

public:
  explicit EvalExprVisitor(std::function<int &(const std::string &)> _fp)
      : fp(std::move(_fp)) {}
  ~EvalExprVisitor() override = default;

  void visit(const IntegerExprAST &ast) override { val = ast.Val; }

  void visit(const VariableExprAST &ast) override {
    val = fp ? fp(ast.Name) : 0;
  }

  void visit(const UnaryExprAST &ast) override {
    ast.rhs->accept(this);
    int rhs = val;
    switch (ast.type) {
    case (ADD):
      val = +rhs;
      break;
    case (SUB):
      val = -rhs;
      break;
    case (INV):
      val = ~rhs;
      break;
    case (NOT):
      val = static_cast<int>(rhs == 0);
      break;
    default:
      throw expr_error("unknown operator");
    }
  }

  void visit(const BinaryExprAST &ast) override {
    ast.lhs->accept(this);
    int lhs = val;
    ast.rhs->accept(this);
    int rhs = val;

    switch (ast.type) {
    case (ADD):
      val = lhs + rhs;
      break;
    case (SUB):
      val = lhs - rhs;
      break;
    case (MUL):
      val = lhs * rhs;
      break;
    case (DIV):
      val = lhs / rhs;
      break;
    case (MOD):
      val = lhs % rhs;
      break;
    case (AND):
      val = lhs & rhs;
      break;
    case (OR):
      val = lhs | rhs;
      break;
    case (XOR):
      val = lhs ^ rhs;
      break;
    case (LAND):
      val = static_cast<int>((lhs != 0) && (rhs != 0));
      break;
    case (LOR):
      val = static_cast<int>((lhs != 0) || (rhs != 0));
      break;
    case (SFTL):
      val = lhs << rhs;
      break;
    case (SFTR):
      val = lhs >> rhs;
      break;
    case (EQ):
      val = static_cast<int>(lhs == rhs);
      break;
    case (NE):
      val = static_cast<int>(lhs != rhs);
      break;
    case (LT):
      val = static_cast<int>(lhs < rhs);
      break;
    case (LE):
      val = static_cast<int>(lhs <= rhs);
      break;
    case (GT):
      val = static_cast<int>(lhs > rhs);
      break;
    case (GE):
      val = static_cast<int>(lhs >= rhs);
      break;
    default:
      throw expr_error("unknown operator");
    }
  }

  void visit(const ConditionalExprAST &ast) override {
    ast.cond->accept(this);
    int cond = val;
    ast.lhs->accept(this);
    int lhs = val;
    ast.rhs->accept(this);
    int rhs = val;
    val = cond != 0 ? lhs : rhs;
  }

  void visit(const AssignExprAST &ast) override {
    ast.rhs->accept(this);
    int rhs = val;

    ast.lhs->accept(this);
    int lhs = val;

    switch (ast.type) {
    case (ASSIGN):
      lhs = rhs;
      break;
    case (ASSIGN_OR):
      lhs |= rhs;
      break;
    case (ASSIGN_XOR):
      lhs ^= rhs;
      break;
    case (ASSIGN_AND):
      lhs &= rhs;
      break;
    case (ASSIGN_SL):
      lhs <<= rhs;
      break;
    case (ASSIGN_SR):
      lhs >>= rhs;
      break;
    case (ASSIGN_ADD):
      lhs += rhs;
      break;
    case (ASSIGN_SUB):
      lhs -= rhs;
      break;
    case (ASSIGN_MUL):
      lhs *= rhs;
      break;
    case (ASSIGN_DIV):
      lhs /= rhs;
      break;
    case (ASSIGN_MOD):
      lhs %= rhs;
      break;
    default:
      throw expr_error("unknown operator");
    }

    if (ast.lhs->type != VAR) {
      throw expr_error("cannot assign to except for variables");
    }
    if (!fp) {
      throw expr_error("not define variavles accesor");
    }

    auto *lhs_ast = dynamic_cast<VariableExprAST *>(ast.lhs.get());
    int &lhs_ref = fp(lhs_ast->Name);
    lhs_ref = val = lhs;
  }

private:
  std::function<int &(const std::string &)> fp;
};

int ExprAST::eval(std::function<int &(const std::string &)> fp) {
  auto evaluator = EvalExprVisitor(std::move(fp));
  accept(&evaluator);
  return evaluator.val;
}

//=============================================================================
// evalute expr_str
std::unique_ptr<ExprAST> parser(const std::string &expr_str) {
  auto tokens = lexer(expr_str);
  return parser(&tokens);
}

//=============================================================================
// evalute expr_str
int eval(const std::string &expr_str,
         std::function<int &(const std::string &)> fp) {
  return parser(expr_str)->eval(std::move(fp));
}

} // namespace expr
