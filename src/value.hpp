/*!
 * \brief value class
 *
 */

#ifndef _VALUE_HPP_
#define _VALUE_HPP_

#include <cstdint>
#include <iostream>

namespace expr {

//=============================================================================
//! \brief value class
class Value {
public:
  explicit Value() { set(0); }

  explicit Value(int32_t val) { set(val); }

  explicit Value(float val) { set(val); }

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
    assignment_operator<std::plus<int>, std::plus<float>>(rhs);
    return *this;
  }

  Value &operator-=(const Value &rhs) {
    assignment_operator<std::minus<int>, std::minus<float>>(rhs);
    return *this;
  }

  Value &operator*=(const Value &rhs) {
    assignment_operator<std::multiplies<int>, std::multiplies<float>>(rhs);
    return *this;
  }

  Value &operator/=(const Value &rhs) {
    assignment_operator<std::divides<int>, std::divides<float>>(rhs);
    return *this;
  }

  Value &operator%=(const Value &rhs) {
    assignment_operator<std::modulus<int>, invalid_operands<float>>(rhs);
    return *this;
  }

  Value &operator<<=(const Value &rhs) {
    assignment_operator<shift_left<int>, invalid_operands<float>>(rhs);
    return *this;
  }

  Value &operator>>=(const Value &rhs) {
    assignment_operator<shift_right<int>, invalid_operands<float>>(rhs);
    return *this;
  }

  Value &operator&=(const Value &rhs) {
    assignment_operator<std::bit_and<int>, invalid_operands<float>>(rhs);
    return *this;
  }

  Value &operator|=(const Value &rhs) {
    assignment_operator<std::bit_or<int>, invalid_operands<float>>(rhs);
    return *this;
  }

  Value &operator^=(const Value &rhs) {
    assignment_operator<std::bit_xor<int>, invalid_operands<float>>(rhs);
    return *this;
  }

  friend Value operator+(Value lhs, const Value &rhs) {
    lhs += rhs;
    return lhs;
  }

  friend Value operator-(Value lhs, const Value &rhs) {
    lhs -= rhs;
    return lhs;
  }

  friend Value operator*(Value lhs, const Value &rhs) {
    lhs *= rhs;
    return lhs;
  }

  friend Value operator/(Value lhs, const Value &rhs) {
    lhs /= rhs;
    return lhs;
  }

  friend Value operator%(Value lhs, const Value &rhs) {
    lhs %= rhs;
    return lhs;
  }

  friend Value operator<<(Value lhs, const Value &rhs) {
    lhs <<= rhs;
    return lhs;
  }

  friend Value operator>>(Value lhs, const Value &rhs) {
    lhs >>= rhs;
    return lhs;
  }

  friend Value operator&(Value lhs, const Value &rhs) {
    lhs &= rhs;
    return lhs;
  }

  friend Value operator|(Value lhs, const Value &rhs) {
    lhs |= rhs;
    return lhs;
  }

  friend Value operator^(Value lhs, const Value &rhs) {
    lhs ^= rhs;
    return lhs;
  }

private:
  union {
    int32_t si32;
    float fp32;
  } data;

  enum Type {
    TY_SI32,
    TY_FP32,
  } type;

  template <typename FI, typename FF>
  void assignment_operator(const Value &rhs) {
    if (is<int>() && rhs.is<int>()) {
      const auto lval = get<int>();
      const auto rval = rhs.get<int>();
      const auto val = FI()(lval, rval);
      set(val);
    } else if (is<float>() && rhs.is<int>()) {
      const auto lval = get<float>();
      const auto rval = static_cast<float>(rhs.get<int>());
      const auto val = FF()(lval, rval);
      set(val);
    } else if (is<int>() && rhs.is<float>()) {
      const auto lval = static_cast<float>(get<int>());
      const auto rval = rhs.get<float>();
      const auto val = FF()(lval, rval);
      set(val);
    } else {
      const auto lval = get<float>();
      const auto rval = rhs.get<float>();
      const auto val = FF()(lval, rval);
      set(val);
    }
  }

  template <typename T> struct invalid_operands {
    T operator()(const T &lhs, const T &rhs) {
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