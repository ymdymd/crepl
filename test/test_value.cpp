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

template <typename TL, typename TR> //
static void test_add() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(2.18f);
  auto expected = lval + rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs + rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, add) {
  SCOPED_TRACE("");
  test_add<int, int>();
  test_add<int, float>();
  test_add<float, int>();
  test_add<float, float>();
}

template <typename TL, typename TR> //
static void test_sub() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(2.18f);
  auto expected = lval - rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs - rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, sub) {
  SCOPED_TRACE("");
  test_sub<int, int>();
  test_sub<int, float>();
  test_sub<float, int>();
  test_sub<float, float>();
}

template <typename TL, typename TR> //
static void test_mul() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(2.18f);
  auto expected = lval * rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs * rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, mul) {
  SCOPED_TRACE("");
  test_mul<int, int>();
  test_mul<int, float>();
  test_mul<float, int>();
  test_mul<float, float>();
}

template <typename TL, typename TR> //
static void test_div() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(2.18f);
  auto expected = lval / rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs / rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, div) {
  SCOPED_TRACE("");
  test_div<int, int>();
  test_div<int, float>();
  test_div<float, int>();
  test_div<float, float>();
}
