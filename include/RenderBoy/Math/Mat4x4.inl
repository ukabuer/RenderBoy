#pragma once
#include "./Mat3x3.hpp"

namespace RB {

template <typename T>
auto Mat4x4<T>::operator==(const Mat4x4 &rhs) const -> bool {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; i < 4; ++i) {
      if ((*this)[i][j] != rhs[i][j]) {
        return false;
      }
    }
  }
  return true;
}

template <typename T>
auto Mat4x4<T>::operator!=(const Mat4x4 &r) const -> bool {
  return !(*this == r);
}

template <typename T> auto Mat4x4<T>::operator*(T rhs) const -> Mat4x4 {
  Mat4x4 out;
  for (size_t i = 0; i < 16; i++) {
    (reinterpret_cast<T *>(&out))[i] = (reinterpret_cast<T *>(this))[i] * rhs;
  }
  return out;
}

template <typename T> auto Mat4x4<T>::operator*=(T rhs) -> Mat4x4 & {
  for (size_t i = 0; i < 16; i++) {
    (reinterpret_cast<T *>(&this))[i] *= rhs;
  }
  return *this;
}

template <typename T>
auto Mat4x4<T>::operator*(const Mat4x4<T> &rhs) const -> Mat4x4 {
  Mat4x4 out;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        out.data[i][j] += this->data[i][k] * rhs.data[k][j];
      }
    }
  }
  return out;
}

template <typename T>
auto Mat4x4<T>::operator*=(const Mat4x4<T> &rhs) -> Mat4x4 & {
  *this = *this * rhs;
  return *this;
}

template <typename T> auto Mat4x4<T>::transposed() const -> Mat4x4 {
  return {_11, _21, _31, _41, _12, _22, _32, _42,
          _13, _23, _33, _43, _14, _24, _34, _44};
}

template <typename T> auto Mat4x4<T>::inversed() const -> Mat4x4 {
  Mat4x4 out;

  // row 1
  out._11 =
      Mat3x3<T>(_22, _23, _24, _32, _33, _34, _42, _43, _44).determinant();

  out._12 =
      -Mat3x3<T>(_21, _23, _24, _31, _33, _34, _41, _43, _44).determinant();

  out._13 =
      Mat3x3<T>(_21, _22, _24, _31, _32, _34, _41, _42, _44).determinant();

  out._14 =
      -Mat3x3<T>(_21, _22, _23, _31, _32, _33, _41, _42, _43).determinant();

  // row 2
  out._21 =
      -Mat3x3<T>(_12, _13, _14, _32, _33, _34, _42, _43, _44).determinant();

  out._22 =
      Mat3x3<T>(_11, _13, _14, _31, _33, _34, _41, _43, _44).determinant();

  out._23 =
      -Mat3x3<T>(_11, _12, _14, _31, _32, _34, _41, _42, _44).determinant();

  out._24 =
      Mat3x3<T>(_11, _12, _13, _31, _32, _33, _41, _42, _43).determinant();

  // row 3
  out._31 =
      Mat3x3<T>(_12, _13, _14, _22, _23, _24, _42, _43, _44).determinant();

  out._32 =
      -Mat3x3<T>(_11, _13, _14, _21, _23, _24, _41, _43, _44).determinant();

  out._33 =
      Mat3x3<T>(_11, _12, _14, _21, _22, _24, _41, _42, _44).determinant();

  out._34 =
      -Mat3x3<T>(_11, _12, _13, _21, _22, _23, _41, _42, _43).determinant();

  // row 4
  out._41 =
      -Mat3x3<T>(_12, _13, _14, _22, _23, _24, _32, _33, _34).determinant();

  out._42 =
      Mat3x3<T>(_11, _13, _14, _21, _23, _24, _31, _33, _34).determinant();

  out._43 =
      -Mat3x3<T>(_11, _12, _14, _21, _22, _24, _31, _32, _34).determinant();

  out._44 =
      Mat3x3<T>(_11, _12, _13, _21, _22, _23, _31, _32, _33).determinant();

  auto det = _11 * out._11 + _12 * out._12 + _13 * out._13 + _14 * out._14;
  out = out.transposed();
  out *= static_cast<T>(1) / det;

  return out;
}

template <typename T> auto Mat4x4<T>::determinant() const -> T {
  T det = 0.f;

  det += _11 *
         Mat3x3<T>(_22, _23, _24, _32, _33, _34, _42, _43, _44).determinant();

  det -= _12 *
         Mat3x3<T>(_21, _23, _24, _31, _33, _34, _41, _43, _44).determinant();

  det += _13 *
         Mat3x3<T>(_21, _22, _24, _31, _32, _34, _41, _42, _44).determinant();

  det -= _14 *
         Mat3x3<T>(_21, _22, _23, _31, _32, _33, _41, _42, _43).determinant();

  return det;
}

template <typename T> auto Mat4x4<T>::Identity() -> Mat4x4 {
  return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

template <typename T> auto Mat4x4<T>::From(const T *values) -> Mat4x4 {
  return {values[0],  values[1],  values[2],  values[3], values[4],  values[5],
          values[6],  values[7],  values[8],  values[9], values[10], values[11],
          values[12], values[13], values[14], values[15]};
}

template <typename T> auto Mat4x4<T>::Translation(T x, T y, T z) -> Mat4x4 {
  // clang-format off
  return {
    1, 0, 0, x,
    0, 1, 0, y,
    0, 0, 1, z,
    0, 0, 0, 1
  };
  // clang-format on
}

template <typename T>
auto Mat4x4<T>::Translation(const Vec3<T> &vec) -> Mat4x4 {
  return Mat4x4<T>::Translation(vec.x, vec.y, vec.z);
}

template <typename T> auto Mat4x4<T>::Scale(T x, T y, T z) -> Mat4x4 {
  // clang-format off
  return {
      x, 0, 0, 0,
      0, y, 0, 0,
      0, 0, z, 0,
      0, 0, 0, 1
  };
  // clang-format on
}

template <typename T> auto Mat4x4<T>::Scale(T s) -> Mat4x4 {
  return Mat4x4<T>::Scale(s, s, s);
}

template <typename T> auto Mat4x4<T>::Scale(const Vec3<T> &vec) -> Mat4x4 {
  return Mat4x4<T>::Scale(vec.x, vec.y, vec.z);
}

template <typename T> auto Mat4x4<T>::RotationX(T radian) -> Mat4x4 {
  auto s = std::sin(radian);
  auto c = std::cos(radian);
  // clang-format off
  return Mat4x4{
      1, 0,  0, 0,
      0, c, -s, 0,
      0, s,  c, 0,
      0, 0,  0, 1
  };
  // clang-format on
}

template <typename T> auto Mat4x4<T>::RotationY(T radian) -> Mat4x4 {
  auto s = std::sin(radian);
  auto c = std::cos(radian);
  // clang-format off
  return Mat4x4{
     c, 0, s, 0,
     0, 0, 0, 0,
    -s, 0, c, 0,
     0, 0, 0, 1
  };
  // clang-format on
}

template <typename T> auto Mat4x4<T>::RotationZ(T radian) -> Mat4x4 {
  auto s = std::sin(radian);
  auto c = std::cos(radian);

  // clang-format off
  return Mat4x4 {
    c, -s, 0, 0,
    s,  c, 0, 0,
    0,  0, 1, 0,
    0,  0, 0, 1
  };
  // clang-format on
}

template <typename T>
auto Mat4x4<T>::Rotation(const Vec3<T> &_axis, T radian) -> Mat4x4 {
  auto axis = _axis.normalized();

  auto s = std::sin(radian);
  auto c = std::cos(radian);
  auto one_minus_cos = 1 - c;

  Mat4x4 out;

  out._11 = 1 + one_minus_cos * (axis.x * axis.x - 1);
  out._21 = axis.z * s + one_minus_cos * axis.x * axis.y;
  out._31 = -axis.y * s + one_minus_cos * axis.x * axis.z;
  out._41 = 0;

  out._12 = -axis.z * s + one_minus_cos * axis.y * axis.x;
  out._22 = 1 + one_minus_cos * (axis.y * axis.y - 1);
  out._32 = axis.x * s + one_minus_cos * axis.y * axis.z;
  out._42 = 0;

  out._13 = axis.y * s + one_minus_cos * axis.z * axis.x;
  out._23 = -axis.x * s + one_minus_cos * axis.z * axis.y;
  out._33 = 1 + one_minus_cos * (axis.z * axis.z - 1);
  out._43 = 0;

  out._14 = 0;
  out._24 = 0;
  out._34 = 0;
  out._44 = 1;

  return out;
}

} // namespace RB