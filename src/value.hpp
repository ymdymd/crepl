/*!
 * \brief value class
 *
 */

#ifndef _VALUE_HPP_
#define _VALUE_HPP_

#include <cstdint>
#include <iostream>

// Suppress warning -Wunused-parameter
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif
namespace expr {

//=============================================================================
//! \brief value class
class Value {
public:
  Value() { set(0); }

  Value(int val) { set(val); }

  Value(float val) { set(val); }

  Value &operator=(const int val) {
    set(val);
    return *this;
  }

  Value &operator=(const float val) {
    set(val);
    return *this;
  }

  //! is non zero value
  bool isNonZero() const {
    return data.si32 != 0;
  }

  //! is value integer?
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
  bool is() const {
    return type == TY_SI32;
  }

  //! is value float?
  template <typename T, typename std::enable_if_t<
                            std::is_floating_point<T>::value, int> = 0>
  bool is() const {
    return type == TY_FP32;
  }

  //! set integer value
  void set(int val) {
    data.si32 = val;
    type = TY_SI32;
  }

  //! set float value
  void set(float val) {
    data.fp32 = val;
    type = TY_FP32;
  }

  //! get integer value if type is integer
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
  T get() const {
    if (!is<T>()) {
      throw;
    }
    return data.si32;
  }

  //! get float value if type is integer
  template <typename T, typename std::enable_if_t<
                            std::is_floating_point<T>::value, int> = 0>
  T get() const {
    if (!is<T>()) {
      throw;
    }
    return data.fp32;
  }

  //! cast to integer type
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
  Value cast() const {
    if (is<float>()) {
      return Value(static_cast<T>(get<float>()));
    }
    return *this;
  }

  //! cast to float type
  template <typename T, typename std::enable_if_t<
                            std::is_floating_point<T>::value, int> = 0>
  Value cast() const {
    if (is<int>()) {
      return Value(static_cast<T>(get<int>()));
    }
    return *this;
  }

  Value &operator+=(const Value &rhs) {
    *this = *this + rhs;
    return *this;
  }

  Value &operator-=(const Value &rhs) {
    *this = *this - rhs;
    return *this;
  }

  Value &operator*=(const Value &rhs) {
    *this = *this * rhs;
    return *this;
  }

  Value &operator/=(const Value &rhs) {
    *this = *this / rhs;
    return *this;
  }

  Value &operator%=(const Value &rhs) {
    *this = *this % rhs;
    return *this;
  }

  Value &operator<<=(const Value &rhs) {
    *this = *this << rhs;
    return *this;
  }

  Value &operator>>=(const Value &rhs) {
    *this = *this >> rhs;
    return *this;
  }

  Value &operator&=(const Value &rhs) {
    *this = *this & rhs;
    return *this;
  }

  Value &operator|=(const Value &rhs) {
    *this = *this | rhs;
    return *this;
  }

  Value &operator^=(const Value &rhs) {
    *this = *this ^ rhs;
    return *this;
  }

  Value operator+() const { return *this; }

  Value operator-() const {
    return unary_operator<std::negate<int>, std::negate<float>>(*this);
  }

  Value operator~() const {
    return unary_operator<std::bit_not<int>, invalid_operand<float>>(*this);
  }

  Value operator!() const {
    return unary_operator<std::logical_not<int>, invalid_operand<float>>(*this);
  }

  friend Value operator+(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::plus<int>, std::plus<float>>(lhs, rhs);
  }

  friend Value operator-(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::minus<int>, std::minus<float>>(lhs, rhs);
  }

  friend Value operator*(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::multiplies<int>, std::multiplies<float>>(lhs, rhs);
  }

  friend Value operator/(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::divides<int>, std::divides<float>>(lhs, rhs);
  }

  friend Value operator%(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::modulus<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator<<(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        shift_left<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator>>(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        shift_right<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator&(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::bit_and<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator|(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::bit_or<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator^(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::bit_xor<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator&&(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::logical_and<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator||(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::logical_or<int>, invalid_operands<float>>(lhs, rhs);
  }

  friend Value operator==(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::equal_to<int>, std::equal_to<float>>(lhs, rhs);
  }

  friend Value operator!=(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::not_equal_to<int>, std::not_equal_to<float>>(lhs, rhs);
  }

  friend Value operator>(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::greater<int>, std::greater<float>>(lhs, rhs);
  }

  friend Value operator>=(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::greater_equal<int>, std::greater_equal<float>>(lhs, rhs);
  }

  friend Value operator<(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::less<int>, std::less<float>>(lhs, rhs);
  }

  friend Value operator<=(const Value &lhs, const Value &rhs) {
    return binary_operator< //
        std::less_equal<int>, std::less_equal<float>>(lhs, rhs);
  }

private:
  union {
    int si32;
    float fp32;
  } data;

  enum Type {
    TY_SI32,
    TY_FP32,
  } type;

  template <typename FI, typename FF>
  static Value binary_operator(const Value &lhs, const Value &rhs) {
    if (lhs.is<int>() && rhs.is<int>()) {
      const auto lval = lhs.get<int>();
      const auto rval = rhs.get<int>();
      const auto val = FI()(lval, rval);
      return Value(val);
    } else if (lhs.is<float>() && rhs.is<int>()) {
      const auto lval = lhs.get<float>();
      const auto rval = static_cast<float>(rhs.get<int>());
      const auto val = FF()(lval, rval);
      return Value(val);
    } else if (lhs.is<int>() && rhs.is<float>()) {
      const auto lval = static_cast<float>(lhs.get<int>());
      const auto rval = rhs.get<float>();
      const auto val = FF()(lval, rval);
      return Value(val);
    } else {
      const auto lval = lhs.get<float>();
      const auto rval = rhs.get<float>();
      const auto val = FF()(lval, rval);
      return Value(val);
    }
  }

  template <typename FI, typename FF>
  static Value unary_operator(const Value &rhs) {
    if (rhs.is<int>()) {
      const auto rval = rhs.get<int>();
      const auto val = FI()(rval);
      return Value(val);
    } else {
      const auto rval = rhs.get<float>();
      const auto val = FF()(rval);
      return Value(val);
    }
  }

  template <typename T> struct invalid_operands {
    T operator()(const T &lhs, const T &rhs) {
      UNUSED(lhs);
      UNUSED(rhs);
      // std::cerr << "invalid operands to binary expression" <<std::endl;
      throw std::runtime_error("invalid operands to binary expression");
      return 0;
    }
  };

  template <typename T> struct invalid_operand {
    T operator()(const T &rhs) {
      UNUSED(rhs);
      // std::cerr << "invalid operands to binary expression" <<std::endl;
      throw std::runtime_error("invalid operands to binary expression");
      return 0;
    }
  };

  template <typename T> struct shift_left {
    T operator()(const T &lhs, const T &rhs) { return lhs << rhs; }
  };

  template <typename T> struct shift_right {
    T operator()(const T &lhs, const T &rhs) { return lhs >> rhs; }
  };
};

} // namespace expr

#endif