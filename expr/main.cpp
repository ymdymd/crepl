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

#define TO_STR(...) #__VA_ARGS__
#define TEST_EVAL(expr_str) ASSERT_EQ( (int)(expr_str), (int)expr::eval(TO_STR(expr_str)))

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
	TEST_EVAL(123 + 456 * 789 + 3 >= 8912 + 3 * 2 ? 3 + 554 * 0 - 1 : 650);

}


std::map<std::string, int> Var;
int getVar(std::string& symbol) {
	return Var[symbol];
}

TEST(eval, variable)
{
	std::string symbol = "hoge";
	Var[symbol] = 2;
	ASSERT_EQ((int)Var[symbol], (int)expr::eval(symbol, getVar));
}


int Reg[16];
int getReg(std::string& symbol) {
	std::smatch m;
	int idx;
	if (regex_search(symbol, m, std::regex(R"(\%[rR]([0-9]+))"))) {
		idx = std::stoul(m[1], nullptr, 10);
	}

	return Reg[idx%(sizeof(Reg) * sizeof(Reg[0]))];

}
TEST(eval, reg)
{

	auto myAST = expr::parser(R"(%r12>=100)");

	Reg[12] = 128;
	ASSERT_EQ((int)(Reg[12]>=100), myAST->eval(getReg));
	Reg[12] = 99;
	ASSERT_EQ((int)(Reg[12] >= 100), myAST->eval(getReg));

}



int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

