#pragma once

#include <memory>
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