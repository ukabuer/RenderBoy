#pragma once

namespace RB {

template <typename T> struct Mat3x3 {
  union {
    struct {
      T _11;
      T _12;
      T _13;
      T _21;
      T _22;
      T _23;
      T _31;
      T _32;
      T _33;
    };
    T data[3][3];
  };

  explicit Mat3x3(T value) {
    _11 = _12 = _13 = _21 = _22 = _23 = _31 = _32 = _33 = value;
  }

  Mat3x3() : Mat3x3(0) {}

  Mat3x3(T v_11, T v_12, T v_13, T v_21, T v_22, T v_23, T v_31, T v_32,
         T v_33) {
    _11 = v_11;
    _12 = v_12;
    _13 = v_13;
    _21 = v_21;
    _22 = v_22;
    _23 = v_23;
    _31 = v_31;
    _32 = v_32;
    _33 = v_33;
  }

  auto operator==(const Mat3x3 &rhs) const -> bool;

  auto operator!=(const Mat3x3 &rhs) const -> bool;

  auto operator*(T v) const -> Mat3x3;

  auto operator*=(T v) -> Mat3x3 &;

  auto operator*(const Mat3x3 &rhs) const -> Mat3x3;

  auto operator*=(const Mat3x3 &rhs) -> Mat3x3 &;

  auto transposed() const -> Mat3x3;

  auto determinant() const -> T;

  static auto Identity() -> Mat3x3;

  static auto From(const T *values) -> Mat3x3;
};

using Mat3x3i = Mat3x3<int32_t>;
using Mat3x3ui = Mat3x3<uint32_t>;
using Mat3x3f = Mat3x3<float>;
using Mat3x3d = Mat3x3<double>;

} // namespace RB

#include "./Mat3x3.inl"