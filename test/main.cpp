#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <memory>
#include <map>
#include "expr.h"


//=============================================================================
//test & main
#include <gtest/gtest.h>


TEST(lexer, basic) {
	std::string str = "1+2*3";
	std::list<expr::Token> expects = {
		{ expr::IMM, "1" },
		{ expr::ADD, "+" },
		{ expr::IMM, "2" },
		{ expr::MUL, "*" },
		{ expr::IMM, "3" },
		{ expr::EOL, "" },
	};

	auto tokens = expr::lexer(str);
	ASSERT_EQ(expects.size(), tokens.size());
	for (auto ite = expects.begin(), ita = tokens.begin(); ite != expects.end() && ita != tokens.end(); ++ite, ++ita) {
		ASSERT_EQ(ite->type, ita->type);
		ASSERT_EQ(ite->str, ita->str);
	}

}


#define TO_STR(...) #__VA_ARGS__
#define TEST_EVAL(expr_str) ASSERT_EQ( (int)(expr_str), (int)expr::eval(TO_STR(expr_str)))


TEST(eval, binary_expression)
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
}

TEST(eval, unary_expression)
{
	TEST_EVAL(+3);
	TEST_EVAL(-3);
	TEST_EVAL(~3);
	TEST_EVAL(!3);
}

TEST(eval, conditional_expression)
{
	TEST_EVAL(3 ? 2 : 1);
	TEST_EVAL(0 ? 1 : 2);
}


TEST(eval, expression)
{
	TEST_EVAL(1 + 2 * 3);
	TEST_EVAL(1 * 2 + 3);
	TEST_EVAL(1 * (2 + 3));
	TEST_EVAL((1 * 2) + 3);
	TEST_EVAL(1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	TEST_EVAL(1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9);
	TEST_EVAL(1 + 2 * 3 + 4 * 5 + 6 * 7 + 8 * 9);
	TEST_EVAL(1 + 2 - 3 + 4 - 5 + 6 - 7 + 8 - 9);
	TEST_EVAL((1 + 2) * (3 + 4) * (5 + 6) * (7 + 8) * 9);
	TEST_EVAL((1 + 2 * 3) + (4 * 5 + 6) * (7 + 8 * 9));
	TEST_EVAL(((1 + 2) * 3) + (4 * (5 + 6)) * ((7 + 8) * 9));
	TEST_EVAL((1 + 2 * 3) ? (4 * 5 + 6) : (7 + 8 * 9));
	TEST_EVAL(1 + 2 ? 3 + 4 : 5 + 6);
	TEST_EVAL(1 + 2 ? 3 + 4 ? 5 + 6 : 7 + 8 : 9);
	TEST_EVAL(123 + 456 * 789 + 3 >= 8912 + 3 * 2 ? 3 + 554 * 0 - 1 : 650);

}


std::map<std::string, int> Var;
int getVar(std::string& symbol, void* _this) {
	return Var[symbol];
}

TEST(eval, variable)
{
	std::string symbol = "hoge";
	Var[symbol] = 2;
	ASSERT_EQ((int)Var[symbol], (int)expr::eval(symbol, getVar));
}


int Reg[16];
int getReg(std::string& symbol, void* _this) {
	std::smatch m;
	int idx;
	if (regex_search(symbol, m, std::regex(R"(\%[rR]([0-9]+))"))) {
		idx = std::stoul(m[1], nullptr, 10);
	}

	return Reg[idx % (sizeof(Reg) * sizeof(Reg[0]))];

}
TEST(eval, reg)
{

	auto myAST = expr::parser(R"(%r12>=100)");

	Reg[12] = 128;
	ASSERT_EQ((int)(Reg[12] >= 100), myAST->eval(getReg));
	Reg[12] = 99;
	ASSERT_EQ((int)(Reg[12] >= 100), myAST->eval(getReg));

}

class Foo {
public:
	int reg[16];
	int getReg(std::string& name) {
		std::smatch m;
		int idx;
		if (regex_search(name, m, std::regex(R"(\%[rR]([0-9]+))"))) {
			idx = std::stoul(m[1], nullptr, 10);
		}
		return reg[idx % (sizeof(reg) * sizeof(reg[0]))];
	}
};

static
int __read_reg(std::string& name, void* _this) {
	std::smatch m;
	int idx;
	if (regex_search(name, m, std::regex(R"(\%[rR]([0-9]+))"))) {
		idx = std::stoul(m[1], nullptr, 10);
	}
	return static_cast <Foo*>(_this)->reg[idx % 16];
}

TEST(eval, this_pointer)
{

	Foo foo;

	auto myAST = expr::parser(R"(%r12>=100)");
	foo.reg[12] = 128;
	ASSERT_EQ((int)(foo.reg[12] >= 100), myAST->eval(__read_reg, &foo));
	foo.reg[12] = 99;
	ASSERT_EQ((int)(foo.reg[12] >= 100), myAST->eval(__read_reg, &foo));

}






int main(int argc, char** argv)
{
#if 0
	//	const std::string expr_str = "(1 + 2) * (3 + 4) * (5 + 6) * (7 + 8) * 9";
	//	const std::string expr_str = "1 + 2 * 3";
	//	const std::string expr_str = "1 * 2 + 3";
	const std::string expr_str = "3? 2:1";
	int val = expr::eval(expr_str);
	std::cout << val << std::endl;
	return val;
#else
#ifdef _DEBUG
	//--gtest_break_on_failure
	testing::GTEST_FLAG(break_on_failure) = true;
#endif
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#endif
}





