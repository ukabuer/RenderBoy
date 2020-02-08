#pragma once
#include "./Vec3.hpp"

namespace RB {

template <typename T> struct Mat4x4 {
  union {
    struct {
      T _11;
      T _12;
      T _13;
      T _14;
      T _21;
      T _22;
      T _23;
      T _24;
      T _31;
      T _32;
      T _33;
      T _34;
      T _41;
      T _42;
      T _43;
      T _44;
    };
    T data[4][4];
  };

  explicit Mat4x4(T value) {
    _11 = _12 = _13 = _14 = value;
    _21 = _22 = _23 = _24 = value;
    _31 = _32 = _33 = _34 = value;
    _41 = _42 = _43 = _44 = value;
  }

  Mat4x4() : Mat4x4(0) {}

  Mat4x4(T v_11, T v_12, T v_13, T v_14, T v_21, T v_22, T v_23, T v_24, T v_31,
         T v_32, T v_33, T v_34, T v_41, T v_42, T v_43, T v_44) {
    _11 = v_11;
    _12 = v_12;
    _13 = v_13;
    _14 = v_14;
    _21 = v_21;
    _22 = v_22;
    _23 = v_23;
    _24 = v_24;
    _31 = v_31;
    _32 = v_32;
    _33 = v_33;
    _34 = v_34;
    _41 = v_41;
    _42 = v_42;
    _43 = v_43;
    _44 = v_44;
  }

  auto operator==(const Mat4x4 &r) const -> bool;

  auto operator!=(const Mat4x4 &r) const -> bool;

  auto operator*(T rhs) const -> Mat4x4;

  auto operator*=(T rhs) -> Mat4x4 &;

  auto operator*(const Mat4x4 &r) const -> Mat4x4;

  auto operator*=(const Mat4x4 &r) -> Mat4x4 &;

  auto transposed() const -> Mat4x4;

  auto inversed() const -> Mat4x4;

  auto determinant() const -> T;

  static auto Identity() -> Mat4x4;

  static auto From(const T *values) -> Mat4x4;

  static auto Translation(T x, T y, T z) -> Mat4x4;

  static auto Translation(const Vec3<T> &vec) -> Mat4x4;

  static auto Scale(T s) -> Mat4x4;

  static auto Scale(T x, T y, T z) -> Mat4x4;

  static auto Scale(const Vec3<T> &vec) -> Mat4x4;

  static auto RotationX(T radian) -> Mat4x4;

  static auto RotationY(T radian) -> Mat4x4;

  static auto RotationZ(T radian) -> Mat4x4;

  static auto Rotation(const Vec3<T> &axis, T radian) -> Mat4x4;

  //  static auto Projection(T fov, T aspect, T z_near, T z_far) -> Mat4x4;
};

template <typename T> auto operator*(T lhs, const Mat4x4<T> &rhs) -> Mat4x4<T>;

using Mat4x4i = Mat4x4<int32_t>;
using Mat4x4ui = Mat4x4<uint32_t>;
using Mat4x4f = Mat4x4<float>;
using Mat4x4d = Mat4x4<double>;

} // namespace RB

#include "./Mat4x4.inl"