#include "value.hpp"
#include <gtest/gtest.h>

using namespace expr;

TEST(value, is_int) {
  Value ival(1);
  ASSERT_TRUE(ival.is<int>());

  Value fval(1.0f);
  ASSERT_FALSE(fval.is<int>());
}

TEST(value, is_float) {
  Value ival(1);
  ASSERT_FALSE(ival.is<float>());

  Value fval(1.0f);
  ASSERT_TRUE(fval.is<float>());
}

TEST(value, cast_i2i) {
  const int org_val = 1;
  auto expect_val = static_cast<int>(org_val);

  Value val(org_val);
  auto res = val.cast<int>();
  ASSERT_TRUE(res.is<int>());
  ASSERT_EQ(expect_val, res.get<int>());
}

TEST(value, cast_i2f) {
  const int org_val = 1;
  auto expect_val = static_cast<float>(org_val);

  Value val(org_val);
  auto res = val.cast<float>();
  ASSERT_TRUE(res.is<float>());
  ASSERT_EQ(expect_val, res.get<float>());
}

TEST(value, cast_f2i) {
  const float org_val = 3.1415f;
  auto expect_val = static_cast<int>(org_val);

  Value val(org_val);
  auto res = val.cast<int>();
  ASSERT_TRUE(res.is<int>());
  ASSERT_EQ(expect_val, res.get<int>());
}

TEST(value, cast_f2f) {
  const float org_val = 1.111f;
  auto expect_val = static_cast<float>(org_val);

  Value val(org_val);
  auto res = val.cast<float>();
  ASSERT_TRUE(res.is<float>());
  ASSERT_EQ(expect_val, res.get<float>());
}

TEST(value, add) {
  const int lval = 1;
  const int rval = 1;
  auto expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_TRUE(res.is<int>());
  ASSERT_EQ(expect_val, res.get<int>());
}

TEST(value, add_fp32_fp32) {
  const float lval = 3.14f;
  const float rval = 2.18f;
  auto expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_TRUE(res.is<float>());
  ASSERT_EQ(expect_val, res.get<float>());
}

TEST(value, add_fp32_si32) {
  const float lval = 3.14f;
  const int rval = 1;
  auto expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_TRUE(res.is<float>());
  ASSERT_EQ(expect_val, res.get<float>());
}

TEST(value, add_si32_fp32) {
  const int lval = 3;
  const float rval = 2.718f;
  const float expect_val = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_TRUE(res.is<float>());
  ASSERT_EQ(expect_val, res.get<float>());
}

TEST(value, sub) {
  const int lval = 3;
  const int rval = 5;
  auto expect_val = lval - rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs - rhs;

  ASSERT_TRUE(res.is<int>());
  ASSERT_EQ(expect_val, res.get<int>());
}

TEST(value, mul) {
  const int lval = 3;
  const int rval = 4;
  auto expect_val = lval * rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs * rhs;

  ASSERT_TRUE(res.is<int>());
  ASSERT_EQ(expect_val, res.get<int>());
}

TEST(value, div) {
  const int lval = 10;
  const int rval = 3;
  auto expect_val = lval / rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs / rhs;

  ASSERT_TRUE(res.is<int>());
  ASSERT_EQ(expect_val, res.get<int>());
}
