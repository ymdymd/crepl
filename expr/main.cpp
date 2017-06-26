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

TEST(eval, parser_eval)
{

	auto myAST = expr::parser("r0>=100 && r1<10 || r2!= 5");

	Var["r0"] = 128;
	Var["r1"] = 22;
	Var["r2"] = 7;
	ASSERT_EQ((int)(Var["r0"] >= 100 && Var["r1"]<10 || Var["r2"] != 5), myAST->eval(getVar));
	Var["r0"] = 1;
	Var["r1"] = 22;
	Var["r2"] = 5;
	ASSERT_EQ((int)(Var["r0"] >= 100 && Var["r1"]<10 || Var["r2"] != 5), myAST->eval(getVar));

}



int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

