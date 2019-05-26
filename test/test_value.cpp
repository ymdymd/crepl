#include "value.hpp"
#include <gtest/gtest.h>

using namespace expr;

TEST(value, add) {
  const int lval = 1;
  const int rval = 1;
  auto expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_EQ(Value::TY_SI32, res.type);
  ASSERT_EQ(expect_val, res.data.si32);
}

TEST(value, add_fp32_fp32) {
  const float lval = 3.14f;
  const float rval = 2.18f;
  auto expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_EQ(Value::TY_FP32, res.type);
  ASSERT_EQ(expect_val, res.data.fp32);
}

TEST(value, add_fp32_si32) {
  const float lval = 3.14f;
  const int rval = 1;
  auto expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_EQ(Value::TY_FP32, res.type);
  ASSERT_EQ(expect_val, res.data.fp32);
}

TEST(value, add_si32_fp32) {
  const int lval = 3;
  const float rval = 2.718f;
  const float expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_EQ(Value::TY_FP32, res.type);
  ASSERT_EQ(expect_val, res.data.fp32);
}

TEST(value, sub) {
  const int lval = 3;
  const int rval = 5;
  auto expect_val = lval - rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs - rhs;

  ASSERT_EQ(Value::TY_SI32, res.type);
  ASSERT_EQ(expect_val, res.data.si32);
}

TEST(value, mul) {
  const int lval = 3;
  const int rval = 4;
  auto expect_val = lval * rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs * rhs;

  ASSERT_EQ(Value::TY_SI32, res.type);
  ASSERT_EQ(expect_val, res.data.si32);
}

TEST(value, div) {
  const int lval = 10;
  const int rval = 3;
  auto expect_val = lval / rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs / rhs;

  ASSERT_EQ(Value::TY_SI32, res.type);
  ASSERT_EQ(expect_val, res.data.si32);
}
