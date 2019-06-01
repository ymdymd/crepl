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
private:
  union {
    int32_t si32;
    float fp32;
  } data;

  enum Type {
    TY_SI32,
    TY_FP32,
  } type;

public:
  explicit Value(int32_t val) {
    data.si32 = val;
    type = TY_SI32;
  }

  explicit Value(float val) {
    data.fp32 = val;
    type = TY_FP32;
  }

  //! is value integer?
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
  bool is() {
    return type == TY_SI32;
  }

  //! is value float?
  template <typename T, typename std::enable_if_t<
                            std::is_floating_point<T>::value, int> = 0>
  bool is() {
    return type == TY_FP32;
  }

  //! get integer value if type is integer
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
  T get() {
    if (!is<T>()) {
      throw;
    }
    return data.si32;
  }

  //! get float value if type is integer
  template <typename T, typename std::enable_if_t<
                            std::is_floating_point<T>::value, int> = 0>
  T get() {
    if (!is<T>()) {
      throw;
    }
    return data.fp32;
  }

  //! cast to integer type
  template <typename T,
            typename std::enable_if_t<std::is_integral<T>::value, int> = 0>
  Value cast() {
    if (is<float>()) {
      return Value(static_cast<T>(get<float>()));
    }
    return *this;
  }

  //! cast to float type
  template <typename T, typename std::enable_if_t<
                            std::is_floating_point<T>::value, int> = 0>
  Value cast() {
    if (is<int>()) {
      return Value(static_cast<T>(get<int>()));
    }
    return *this;
  }

  Value &operator+=(const Value &rhs) {
    const auto lty = type;
    const auto rty = rhs.type;

    if ((lty == TY_SI32) && (rty == TY_SI32)) {
      const auto lval = data.si32;
      const auto rval = rhs.data.si32;
      (*this).data.si32 = lval + rval;
      (*this).type = TY_SI32;
    } else if ((lty == TY_FP32) && (rty == TY_SI32)) {
      const auto lval = data.fp32;
      const auto rval = static_cast<float>(rhs.data.si32);
      (*this).data.fp32 = lval + rval;
      (*this).type = TY_FP32;
    } else if ((lty == TY_SI32) && (rty == TY_FP32)) {
      const auto lval = static_cast<float>(data.si32);
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval + rval;
      (*this).type = TY_FP32;
    } else {
      const auto lval = data.fp32;
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval + rval;
      (*this).type = TY_FP32;
    }
    return *this;
  }

  Value &operator-=(const Value &rhs) {
    const auto lty = type;
    const auto rty = rhs.type;

    if ((lty == TY_SI32) && (rty == TY_SI32)) {
      const auto lval = data.si32;
      const auto rval = rhs.data.si32;
      (*this).data.si32 = lval - rval;
      (*this).type = TY_SI32;
    } else if ((lty == TY_FP32) && (rty == TY_SI32)) {
      const auto lval = data.fp32;
      const auto rval = static_cast<float>(rhs.data.si32);
      (*this).data.fp32 = lval - rval;
      (*this).type = TY_FP32;
    } else if ((lty == TY_SI32) && (rty == TY_FP32)) {
      const auto lval = static_cast<float>(data.si32);
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval - rval;
      (*this).type = TY_FP32;
    } else {
      const auto lval = data.fp32;
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval - rval;
      (*this).type = TY_FP32;
    }
    return *this;
  }

  Value &operator*=(const Value &rhs) {
    const auto lty = type;
    const auto rty = rhs.type;

    if ((lty == TY_SI32) && (rty == TY_SI32)) {
      const auto lval = data.si32;
      const auto rval = rhs.data.si32;
      (*this).data.si32 = lval * rval;
      (*this).type = TY_SI32;
    } else if ((lty == TY_FP32) && (rty == TY_SI32)) {
      const auto lval = data.fp32;
      const auto rval = static_cast<float>(rhs.data.si32);
      (*this).data.fp32 = lval * rval;
      (*this).type = TY_FP32;
    } else if ((lty == TY_SI32) && (rty == TY_FP32)) {
      const auto lval = static_cast<float>(data.si32);
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval * rval;
      (*this).type = TY_FP32;
    } else {
      const auto lval = data.fp32;
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval * rval;
      (*this).type = TY_FP32;
    }
    return *this;
  }

  Value &operator/=(const Value &rhs) {
    const auto lty = type;
    const auto rty = rhs.type;

    if ((lty == TY_SI32) && (rty == TY_SI32)) {
      const auto lval = data.si32;
      const auto rval = rhs.data.si32;
      (*this).data.si32 = lval / rval;
      (*this).type = TY_SI32;
    } else if ((lty == TY_FP32) && (rty == TY_SI32)) {
      const auto lval = data.fp32;
      const auto rval = static_cast<float>(rhs.data.si32);
      (*this).data.fp32 = lval / rval;
      (*this).type = TY_FP32;
    } else if ((lty == TY_SI32) && (rty == TY_FP32)) {
      const auto lval = static_cast<float>(data.si32);
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval / rval;
      (*this).type = TY_FP32;
    } else {
      const auto lval = data.fp32;
      const auto rval = rhs.data.fp32;
      (*this).data.fp32 = lval / rval;
      (*this).type = TY_FP32;
    }
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
};

} // namespace expr

#endif