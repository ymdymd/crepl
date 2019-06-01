#include "expr.h"
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string>

//=============================================================================
// test & main
#include <gtest/gtest.h>

//=============================================================================

#if 1
int main(int argc, char **argv) {
// #ifdef _DEBUG
#if 1
  //--gtest_break_on_failure
  testing::GTEST_FLAG(break_on_failure) = true;
#endif
  // ::testing::GTEST_FLAG(filter) = "value*";

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#else
int main(int argc, char **argv) {
  //	const std::string expr_str = "(1 + 2) * (3 + 4) * (5 + 6) * (7 + 8) *
  //9";
  //	const std::string expr_str = "1 + 2 * 3";
  //	const std::string expr_str = "1 * 2 + 3";
  // const std::string expr_str = "3? 2:1";
  // const std::string expr_str = "0xDeadBeaf";
  const std::string expr_str = "0xdeadbeaf";
  auto tokens = expr::lexer(expr_str);
  for (auto itr = tokens.begin(); itr != tokens.end(); ++itr) {
    std::cout << itr->str << std::endl;
  }

  auto ast = expr::parser(tokens);
  int val = ast->eval();
  std::cout << val << std::endl;
  return val;
}
#endif
