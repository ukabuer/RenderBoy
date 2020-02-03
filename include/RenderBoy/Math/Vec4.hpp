#pragma once

namespace RB {

template <typename T> struct Vec4 {
  using value_type = T;

  union {
    struct {
      T x;
      T y;
      T z;
      T w;
    };
    struct {
      T r;
      T g;
      T b;
      T a;
    };
  };

  Vec4() : x(0), y(0), z(0), w(0) {}

  explicit Vec4(T value) : x(value), y(value), z(value), w(value) {}

  Vec4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

  auto operator+(const Vec4 &rhs) const -> Vec4;

  auto operator+=(const Vec4 &rhs) -> Vec4 &;

  auto operator-(const Vec4 &rhs) const -> Vec4;

  auto operator-=(const Vec4 &rhs) -> Vec4 &;

  auto operator-() const -> Vec4;

  auto operator*(const Vec4 &rhs) const -> Vec4;

  auto operator*(T rhs) const -> Vec4;

  auto operator*=(const Vec4 &rhs) -> Vec4 &;

  auto operator*=(T rhs) -> Vec4 &;

  auto operator/(const Vec4 &rhs) const -> Vec4;

  auto operator/(T rhs) const -> Vec4;

  auto operator/=(const Vec4 &rhs) -> Vec4 &;

  auto operator/=(T rhs) -> Vec4 &;

  auto operator==(const Vec4 &rhs) const -> bool;

  auto operator!=(const Vec4 &rhs) const -> bool;

  auto operator[](uint32_t index) const -> const T &;

  auto operator[](uint32_t index) -> T &;

  auto normalize() -> Vec4 &;

  auto normalized() const -> Vec4;

  auto square_norm() const -> T;

  auto norm() const -> T;

  template <typename U> auto cast() const -> Vec4<U>;

  static auto From(const T *data) -> Vec4;
};

template <typename T> auto operator*(T lhs, const Vec4<T> &rhs) -> Vec4<T>;

using Vec4f = Vec4<float>;
using Vec4i = Vec4<int32_t>;
using Vec4ui = Vec4<uint32_t>;

} // namespace RB

#include "./Vec4.inl"