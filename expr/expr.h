#pragma once

#include <memory>
#include <string>

namespace expr {

//-----------------------------------------------------------------------------
// AST (Abstract Syntax Tree)
class ExprAST {
public:
	virtual ~ExprAST() {}
	virtual int eval(int(*fp)(std::string&) = nullptr) { return 0; }
};

//-----------------------------------------------------------------------------
// evalute expr_str
std::unique_ptr<ExprAST> parser(const std::string expr_str);

//-----------------------------------------------------------------------------
// evalute expr_str
int eval(const std::string expr_str, int(*fp)(std::string&) = nullptr);

}	//namespace expr