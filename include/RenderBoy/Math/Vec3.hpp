#pragma once

namespace RB {

template <typename T> struct Vec3 {
  using value_type = T;

  union {
    struct {
      T x;
      T y;
      T z;
    };
    struct {
      T r;
      T g;
      T b;
    };
    struct {
      T u;
      T v;
      T w;
    };
  };

  Vec3() : x(0), y(0), z(0) {}

  explicit Vec3(T value) : x(value), y(value), z(value) {}

  Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

  auto operator+(const Vec3 &rhs) const -> Vec3;

  auto operator+=(const Vec3 &rhs) -> Vec3 &;

  auto operator-(const Vec3 &rhs) const -> Vec3;

  auto operator-=(const Vec3 &rhs) -> Vec3 &;

  auto operator-() const -> Vec3;

  auto operator*(const Vec3 &rhs) const -> Vec3;

  auto operator*(T rhs) const -> Vec3;

  auto operator*=(const Vec3 &rhs) -> Vec3 &;

  auto operator*=(T rhs) -> Vec3 &;

  auto operator/(const Vec3 &rhs) const -> Vec3;

  auto operator/(T rhs) const -> Vec3;

  auto operator/=(const Vec3 &rhs) -> Vec3 &;

  auto operator/=(T rhs) -> Vec3 &;

  auto operator==(const Vec3 &rhs) const -> bool;

  auto operator!=(const Vec3 &rhs) const -> bool;

  auto operator[](uint32_t index) const -> const T &;

  auto operator[](uint32_t index) -> T &;

  auto normalize() -> Vec3 &;

  auto normalized() const -> Vec3;

  auto square_norm() const -> T;

  auto norm() const -> T;

  template <typename U> auto cast() const -> Vec3<U>;

  static auto From(const T *data) -> Vec3;
};

template <typename T> auto operator*(T lhs, const Vec3<T> &rhs) -> Vec3<T>;

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int32_t>;
using Vec3ui = Vec3<uint32_t>;

} // namespace RB

#include "./Vec3.inl"