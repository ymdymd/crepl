#pragma once

#include <memory>
#include <list>
#include <string>


//-----------------------------------------------------------------------------
/*
## Evaluation of expressions
```cpp
	int a = expr::eval("1+2+3");									// a = 1+2+3
	int b = expr::eval("(1 + 2 * 3) ? (4 * 5 + 6) : (7 + 8 * 9)");	// b = (1 + 2 * 3) ? (4 * 5 + 6) : (7 + 8 * 9)

```
## Evaluation of expressions (with symbol)
```cpp
	std::map<std::string, int> SymbolMap;
	int getVar(std::string& symbol) {
		return SymbolMap[symbol];
	}

	std::string symbol = "foo";
	SymbolMap[symbol] = 2;
	int bar = expr::eval(symbol, getVar));		//bar = foo = 2
```

## Create Abstract Syntax Tree
```cpp
	std::map<std::string, int> M;
	int getVar(std::string& symbol) {return M[symbol];}
	M["r0"] = 128;
	M["r1"] = 22;
	M["r2"] = 7;

	auto myAST = expr::parser("r0>=100 && r1<10 || r2!= 5");
	int cond = myAST->eval(getVar);
```
*/

namespace expr {

//=============================================================================
// enum/struct/class

//-----------------------------------------------------------------------------
// token / node type definition
enum Type {
	INVALID = -1,	// invalid token
	EOL = 0,		// End Of Line must be 0.
	IMM,			//	  integer_expression	
	VAR,			//    variable_expression
	REG,			//	  register_variable_expression	
	PARL,			// (  parenthesis_expression 
	PARR,			// )  parenthesis_expression
	QUESTION,		// ?  conditional_expression
	SEMICOLON,		// :
	COLON,			// ;

	//assignment_expression 
	ASSIGN_BIGIN,
	ASSIGN,			// =  assignment_expression
	ASSIGN_OR,		// |=  assignment_expression
	ASSIGN_XOR,		// ^=  assignment_expression
	ASSIGN_AND,		// &=  assignment_expression
	ASSIGN_SL,		// <<=  assignment_expression
	ASSIGN_SR,		// >>=  assignment_expression
	ASSIGN_ADD,		// +=  assignment_expression
	ASSIGN_SUB,		// -=  assignment_expression
	ASSIGN_MUL,		// *=  assignment_expression
	ASSIGN_DIV,		// /=  assignment_expression
	ASSIGN_MOD,		// %=  assignment_expression
	ASSIGN_END,

	//binary_ops_expression 
	BINOP_BIGIN,
	LOR,			// || logical_or_expression
	LAND,			// && logical_and_expression
	OR,				// |  inclusive_or_expression
	XOR,			// ^  exclusive_or_expression
	AND,			// &  and_expression
	EQ,				// == equality_expression
	NE,				// != equality_expression
	LT,				// <  relational_expression
	LE,				// <= relational_expression
	GT,				// >  relational_expression
	GE,				// >= relational_expression
	SFTL,			// << shift_expression
	SFTR,			// >> shift_expression
	ADD,			// +  additive_expression
	SUB,			// -  additive_expression 
	MUL,			// *  multiplicative_expression
	DIV,			// /  multiplicative_expression
	MOD,			// %  multiplicative_expression
	BINOP_END,

	//unary_expression  
	INV,			// ~  unary_expression 
	NOT,			// !  unary_expression

};

//-----------------------------------------------------------------------------
// token struct
struct Token {
	Type type;			//token type
	std::string str;	//token string
	Token(void) : type(EOL), str("") {};
	Token(Type _type, std::string _str) : type(_type), str(_str) {};
};



//-----------------------------------------------------------------------------
// AST (Abstract Syntax Tree)
class ExprAST {
public:
	const Type type;		//node type
	ExprAST(const Type _type) : type(_type){}
	virtual ~ExprAST() {}
	virtual int eval(int&(*fp)(const std::string&, void*) = nullptr, void* _this = nullptr) = 0;
};


//-----------------------------------------------------------------------------
// exception
typedef std::runtime_error expr_error;

//class expr_error : public std::runtime_error {
//};

//=============================================================================
// functions

//-----------------------------------------------------------------------------
// lexer lexical analyzer
std::list<Token> lexer(const std::string& line);

//-----------------------------------------------------------------------------
// parser
std::unique_ptr<ExprAST> parser(std::list<Token>& tokens);
std::unique_ptr<ExprAST> parser(const std::string& expr_str);

//-----------------------------------------------------------------------------
// evalute expr_str
int eval(const std::string expr_str, int&(*fp)(const std::string&, void*) = nullptr, void* _this = nullptr);

}	//namespace expr