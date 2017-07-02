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

void test_lexer_helper(std::list<expr::Token>& expects) {
	std::string str;
	for (auto ite = expects.begin(); ite != expects.end(); ++ite) {
		str += ite->str;
	}
	auto tokens = expr::lexer(str);
	ASSERT_EQ(expects.size(), tokens.size());
	for (auto ite = expects.begin(), ita = tokens.begin(); ite != expects.end() && ita != tokens.end(); ++ite, ++ita) {
		ASSERT_EQ(ite->type, ita->type);
		ASSERT_EQ(ite->str, ita->str);
	}
}

void test_lexer_helper(std::list<std::string> expects) {
	std::string str;
	for (auto ite = expects.begin(); ite != expects.end(); ++ite) {
		str += *ite + " ";
	}
	auto tokens = expr::lexer(str);
	ASSERT_EQ(expects.size()+1, tokens.size());
	auto ita = tokens.begin();
	for (auto ite = expects.begin() ; ite != expects.end() && ita != tokens.end(); ++ite, ++ita) {
		ASSERT_EQ(*ite, ita->str);
	}
}

TEST(lexer, words) {
	test_lexer_helper({ "a" });
	test_lexer_helper({ "hoge" });
	test_lexer_helper({ "20170630" });
	test_lexer_helper({ "this","is","a","pen" });
}


TEST(lexer, basic) {
	std::list<expr::Token> expects = {
		{ expr::IMM, "1" },
		{ expr::ADD, "+" },
		{ expr::IMM, "2" },
		{ expr::MUL, "*" },
		{ expr::IMM, "3" },
		{ expr::EOL, "" },
	};
	test_lexer_helper(expects);
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


int a, b, c, d, e, f, g = 0;

int& getVar(const std::string& symbol, void* _this) {
	if (symbol == "a") return a;
	if (symbol == "b") return b;
	if (symbol == "c") return c;
	if (symbol == "d") return d;
	if (symbol == "e") return e;
	if (symbol == "f") return f;
	if (symbol == "g") return g;
	return a;
}

#define TEST_VAR(expr_str) ASSERT_EQ((int)(expr_str), (int)expr::eval(TO_STR(expr_str), getVar))

TEST(eval, variable_expression)
{
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	e = 5;
	f = 6;
	g = 7;

	TEST_VAR(a);
	TEST_VAR(b);
	TEST_VAR(c);
	TEST_VAR(d);
	TEST_VAR(e);
	TEST_VAR(f);
	TEST_VAR(g);
}

TEST(eval, assign_expression)
{
	TEST_VAR(a = 2);
	TEST_VAR(b = 3);
	TEST_VAR(c = 4);
	TEST_VAR(d = a);
	TEST_VAR(e = b);
	TEST_VAR(f = c);
	TEST_VAR(g = a = b = c = d = e = f) ;
}

TEST(eval, expression2)
{
	a = 1;
	b = 2;
	c = 3;
	d = 4;
	e = 5;
	f = 6;
	TEST_VAR(g = a + b + c + d + e + f);
	TEST_VAR(g = a + b * c + d * e + f);
	TEST_VAR(g = a * b + c * d + e * f);
	TEST_VAR(g = (a + b) * (c + d) * (e + f));

}




int Reg[16];
int& getReg(const std::string& symbol, void* _this) {
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
	int& getReg(const std::string& name) {
		std::smatch m;
		int idx;
		if (regex_search(name, m, std::regex(R"(\%[rR]([0-9]+))"))) {
			idx = std::stoul(m[1], nullptr, 10);
		}
		return reg[idx % (sizeof(reg) * sizeof(reg[0]))];
	}
};

static
int& __read_reg(const std::string& name, void* _this) {
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
	//const std::string expr_str = "3? 2:1";
	const std::string expr_str = "1+2";
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





