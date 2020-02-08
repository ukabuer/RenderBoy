#pragma once
#include <cassert>

namespace RB {

template <typename T> auto Vec2<T>::operator+(const Vec2 &rhs) const -> Vec2 {
  return {x + rhs.x, y + rhs.y};
}

template <typename T> auto Vec2<T>::operator+=(const Vec2 &rhs) -> Vec2 & {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

template <typename T> auto Vec2<T>::operator-(const Vec2 &rhs) const -> Vec2 {
  return {x - rhs.x, y - rhs.y};
}

template <typename T> auto Vec2<T>::operator-() const -> Vec2 {
  return {-x, -y};
}

template <typename T> auto Vec2<T>::operator-=(const Vec2 &rhs) -> Vec2 & {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

template <typename T> auto Vec2<T>::operator*(const Vec2 &rhs) const -> Vec2 {
  return {x * rhs.x, y * rhs.y};
}

template <typename T> auto Vec2<T>::operator*=(const Vec2 &rhs) -> Vec2 & {
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

template <typename T> auto Vec2<T>::operator*(T rhs) const -> Vec2 {
  return {x * rhs, y * rhs};
}

template <typename T> auto Vec2<T>::operator*=(T rhs) -> Vec2 & {
  x *= rhs;
  y *= rhs;
  return *this;
}

template <typename T> auto Vec2<T>::operator/(const Vec2 &rhs) const -> Vec2 {
  assert(rhs.x != 0 && rhs.y != 0);
  return {x / rhs.x, y / rhs.y};
}

template <typename T> auto Vec2<T>::operator/(T rhs) const -> Vec2 {
  assert(rhs != 0);
  return {x / rhs, y / rhs};
}

template <typename T> auto Vec2<T>::operator/=(const Vec2 &rhs) -> Vec2 & {
  assert(rhs.x != 0 && rhs.y != 0);
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

template <typename T> auto Vec2<T>::operator/=(T rhs) -> Vec2 & {
  assert(rhs != 0);
  x /= rhs;
  y /= rhs;
  return *this;
}

template <typename T> auto Vec2<T>::operator==(const Vec2 &rhs) const -> bool {
  return x == rhs.x && y == rhs.y;
}

template <typename T> auto Vec2<T>::operator!=(const Vec2 &rhs) const -> bool {
  return !(*this == rhs);
}

template <typename T>
auto Vec2<T>::operator[](uint32_t index) const -> const T & {
  assert(index < 2);
  return reinterpret_cast<const T *>(this)[index];
}

template <typename T> auto Vec2<T>::operator[](uint32_t index) -> T & {
  assert(index < 2);
  return reinterpret_cast<T *>(this)[index];
}

template <typename T> auto Vec2<T>::normalize() -> Vec2 & {
  auto norm = this->norm();
  if (norm == 0)
    return *this;

  (*this) /= norm;
  return *this;
}

template <typename T> auto Vec2<T>::normalized() const -> Vec2 {
  auto norm = this->norm();
  if (norm == 0)
    return *this;

  return (*this) / norm;
}

template <typename T> auto Vec2<T>::square_norm() const -> T {
  return x * x + y * y;
}

template <typename T> auto Vec2<T>::norm() const -> T {
  return std::sqrt(this->square_norm());
}

template <typename T>
template <typename U>
auto Vec2<T>::cast() const -> Vec2<U> {
  return {static_cast<U>(x), static_cast<U>(y)};
}

template <typename T> auto Vec2<T>::From(const T *data) -> Vec2 {
  return {data[0], data[1]};
}

template <typename T> auto operator*(T lhs, const Vec2<T> &rhs) -> Vec2<T> {
  return rhs * lhs;
}

} // namespace RB