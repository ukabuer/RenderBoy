#pragma once

namespace RB {

template <typename T> struct Vec2 {
  using value_type = T;

  union {
    struct {
      T x;
      T y;
    };
    struct {
      T u;
      T v;
    };
    struct {
      T s;
      T t;
    };
  };

  Vec2() : x(0), y(0) {}

  explicit Vec2(T value) : x(value), y(value) {}

  Vec2(T _x, T _y) : x(_x), y(_y) {}

  auto operator+(const Vec2 &rhs) const -> Vec2;

  auto operator+=(const Vec2 &rhs) -> Vec2 &;

  auto operator-(const Vec2 &rhs) const -> Vec2;

  auto operator-=(const Vec2 &rhs) -> Vec2 &;

  auto operator-() const -> Vec2;

  auto operator*(const Vec2 &rhs) const -> Vec2;

  auto operator*(T rhs) const -> Vec2;

  auto operator*=(const Vec2 &rhs) -> Vec2 &;

  auto operator*=(T rhs) -> Vec2 &;

  auto operator/(const Vec2 &rhs) const -> Vec2;

  auto operator/(T rhs) const -> Vec2;

  auto operator/=(const Vec2 &rhs) -> Vec2 &;

  auto operator/=(T rhs) -> Vec2 &;

  auto operator==(const Vec2 &rhs) const -> bool;

  auto operator!=(const Vec2 &rhs) const -> bool;

  auto operator[](uint32_t index) const -> const T &;

  auto operator[](uint32_t index) -> T &;

  auto normalize() -> Vec2 &;

  auto normalized() const -> Vec2;

  auto square_norm() const -> T;

  auto norm() const -> T;

  template <typename U> auto cast() const -> Vec2<U>;

  static auto From(const T *data) -> Vec2;
};

template <typename T> auto operator*(T lhs, const Vec2<T> &rhs) -> Vec2<T>;

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int32_t>;
using Vec2ui = Vec2<uint32_t>;

} // namespace RB

#include "./Vec2.inl"