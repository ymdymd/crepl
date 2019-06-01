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

template <typename TL, typename TR> //
static void test_mod() {
  auto lval = static_cast<TL>(10);
  auto rval = static_cast<TR>(3);
  auto expected = lval % rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs % rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

template <typename TL, typename TR> //
static void test_mod_invalid() {
  auto lval = static_cast<TL>(10);
  auto rval = static_cast<TR>(3);
  // auto expected = lval % rval;

  Value lhs(lval);
  Value rhs(rval);
  Value res;
  ASSERT_ANY_THROW(res = lhs % rhs;);
}

TEST(value, mod) {
  SCOPED_TRACE("");
  test_mod<int, int>();
  test_mod_invalid<int, float>();
  test_mod_invalid<float, int>();
  test_mod_invalid<float, float>();
}

template <typename TL, typename TR> //
static void test_sl() {
  auto lval = static_cast<TL>(111);
  auto rval = static_cast<TR>(3);
  auto expected = lval << rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs << rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

template <typename TL, typename TR> //
static void test_sl_invalid() {
  auto lval = static_cast<TL>(111);
  auto rval = static_cast<TR>(3);
  // auto expected = lval << rval;

  Value lhs(lval);
  Value rhs(rval);
  Value res;
  ASSERT_ANY_THROW(res = lhs << rhs;);
}

TEST(value, sl) {
  SCOPED_TRACE("");
  test_sl<int, int>();
  test_sl_invalid<int, float>();
  test_sl_invalid<float, int>();
  test_sl_invalid<float, float>();
}

template <typename TL, typename TR> //
static void test_sr() {
  auto lval = static_cast<TL>(111);
  auto rval = static_cast<TR>(3);
  auto expected = lval >> rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs >> rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

template <typename TL, typename TR> //
static void test_sr_invalid() {
  auto lval = static_cast<TL>(111);
  auto rval = static_cast<TR>(3);
  // auto expected = lval >> rval;

  Value lhs(lval);
  Value rhs(rval);
  Value res;
  ASSERT_ANY_THROW(res = lhs >> rhs;);
}

TEST(value, sr) {
  SCOPED_TRACE("");
  test_sr<int, int>();
  test_sr_invalid<int, float>();
  test_sr_invalid<float, int>();
  test_sr_invalid<float, float>();
}

template <typename TL, typename TR> //
static void test_and() {
  auto lval = static_cast<TL>(0xdeadbeaf);
  auto rval = static_cast<TR>(0xcafebabe);
  auto expected = lval & rval;
  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs & rhs;
  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

template <typename TL, typename TR> //
static void test_and_invalid() {
  auto lval = static_cast<TL>(0xdeadbeaf);
  auto rval = static_cast<TR>(0xcafebabe);
  Value lhs(lval);
  Value rhs(rval);
  Value res;
  ASSERT_ANY_THROW(res = lhs & rhs;);
}

TEST(value, and) {
  SCOPED_TRACE("");
  test_and<int, int>();
  test_and_invalid<int, float>();
  test_and_invalid<float, int>();
  test_and_invalid<float, float>();
}

template <typename TL, typename TR> //
static void test_or() {
  auto lval = static_cast<TL>(0xdeadbeaf);
  auto rval = static_cast<TR>(0xcafebabe);
  auto expected = lval | rval;
  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs | rhs;
  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

template <typename TL, typename TR> //
static void test_or_invalid() {
  auto lval = static_cast<TL>(0xdeadbeaf);
  auto rval = static_cast<TR>(0xcafebabe);
  Value lhs(lval);
  Value rhs(rval);
  Value res;
  ASSERT_ANY_THROW(res = lhs | rhs;);
}

TEST(value, or) {
  SCOPED_TRACE("");
  test_or<int, int>();
  test_or_invalid<int, float>();
  test_or_invalid<float, int>();
  test_or_invalid<float, float>();
}

template <typename TL, typename TR> //
static void test_xor() {
  auto lval = static_cast<TL>(0xdeadbeaf);
  auto rval = static_cast<TR>(0xcafebabe);
  auto expected = lval ^ rval;
  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs ^ rhs;
  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

template <typename TL, typename TR> //
static void test_xor_invalid() {
  auto lval = static_cast<TL>(0xdeadbeaf);
  auto rval = static_cast<TR>(0xcafebabe);
  Value lhs(lval);
  Value rhs(rval);
  Value res;
  ASSERT_ANY_THROW(res = lhs ^ rhs;);
}

TEST(value, xor) {
  SCOPED_TRACE("");
  test_xor<int, int>();
  test_xor_invalid<int, float>();
  test_xor_invalid<float, int>();
  test_xor_invalid<float, float>();
}

template <typename TL, typename TR> //
static void test_cmpeq() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(3.0f);
  auto expected = lval == rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs == rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, cmpeq) {
  SCOPED_TRACE("");
  test_cmpeq<int, int>();
  test_cmpeq<int, float>();
  test_cmpeq<float, int>();
  test_cmpeq<float, float>();
}

template <typename TL, typename TR> //
static void test_cmpne() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(3.0f);
  auto expected = lval != rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs != rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, cmpne) {
  SCOPED_TRACE("");
  test_cmpne<int, int>();
  test_cmpne<int, float>();
  test_cmpne<float, int>();
  test_cmpne<float, float>();
}

template <typename TL, typename TR> //
static void test_cmpgt() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(3.0f);
  auto expected = lval > rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs > rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, cmpgt) {
  SCOPED_TRACE("");
  test_cmpgt<int, int>();
  test_cmpgt<int, float>();
  test_cmpgt<float, int>();
  test_cmpgt<float, float>();
}

template <typename TL, typename TR> //
static void test_cmpge() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(3.0f);
  auto expected = lval >= rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs >= rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, cmpge) {
  SCOPED_TRACE("");
  test_cmpge<int, int>();
  test_cmpge<int, float>();
  test_cmpge<float, int>();
  test_cmpge<float, float>();
}

template <typename TL, typename TR> //
static void test_cmplt() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(3.0f);
  auto expected = lval < rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs < rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, cmplt) {
  SCOPED_TRACE("");
  test_cmplt<int, int>();
  test_cmplt<int, float>();
  test_cmplt<float, int>();
  test_cmplt<float, float>();
}

template <typename TL, typename TR> //
static void test_cmple() {
  auto lval = static_cast<TL>(3.14f);
  auto rval = static_cast<TR>(3.0f);
  auto expected = lval <= rval;

  Value lhs(lval);
  Value rhs(rval);
  auto res = lhs <= rhs;

  ASSERT_TRUE(res.is<decltype(expected)>());
  ASSERT_EQ(expected, res.get<decltype(expected)>());
}

TEST(value, cmple) {
  SCOPED_TRACE("");
  test_cmple<int, int>();
  test_cmple<int, float>();
  test_cmple<float, int>();
  test_cmple<float, float>();
}
