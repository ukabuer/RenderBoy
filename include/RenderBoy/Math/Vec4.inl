#pragma once
#include <cassert>

namespace RB {

template <typename T> auto Vec4<T>::operator+(const Vec4 &rhs) const -> Vec4 {
  return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
}

template <typename T> auto Vec4<T>::operator+=(const Vec4 &rhs) -> Vec4 & {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

template <typename T> auto Vec4<T>::operator-(const Vec4 &rhs) const -> Vec4 {
  return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
}

template <typename T> auto Vec4<T>::operator-() const -> Vec4 {
  return {-x, -y, -z, -w};
}

template <typename T> auto Vec4<T>::operator-=(const Vec4 &rhs) -> Vec4 & {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

template <typename T> auto Vec4<T>::operator*(const Vec4 &rhs) const -> Vec4 {
  return {x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w};
}

template <typename T> auto Vec4<T>::operator*=(const Vec4 &rhs) -> Vec4 & {
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  w *= rhs.w;
  return *this;
}

template <typename T> auto Vec4<T>::operator*(T rhs) const -> Vec4 {
  return {x * rhs, y * rhs, z * rhs.z, w * rhs.w};
}

template <typename T> auto Vec4<T>::operator*=(T rhs) -> Vec4 & {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

template <typename T> auto Vec4<T>::operator/(const Vec4 &rhs) const -> Vec4 {
  assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
  return {x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w};
}

template <typename T> auto Vec4<T>::operator/(T rhs) const -> Vec4 {
  assert(rhs != 0);
  return {x / rhs, y / rhs, z / rhs, w / rhs};
}

template <typename T> auto Vec4<T>::operator/=(const Vec4 &rhs) -> Vec4 & {
  assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  w /= rhs.w;
  return *this;
}

template <typename T> auto Vec4<T>::operator/=(T rhs) -> Vec4 & {
  assert(rhs != 0);
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}

template <typename T> auto Vec4<T>::operator==(const Vec4 &rhs) const -> bool {
  return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

template <typename T> auto Vec4<T>::operator!=(const Vec4 &rhs) const -> bool {
  return !(*this == rhs);
}

template <typename T>
auto Vec4<T>::operator[](uint32_t index) const -> const T & {
  assert(index < 4);
  return reinterpret_cast<const T *>(this)[index];
}

template <typename T> auto Vec4<T>::operator[](uint32_t index) -> T & {
  assert(index < 4);
  return reinterpret_cast<T *>(this)[index];
}

template <typename T> auto Vec4<T>::normalize() -> Vec4 & {
  auto norm = this->norm();
  if (norm == 0)
    return;

  (*this) /= norm;
  return *this;
}

template <typename T> auto Vec4<T>::normalized() const -> Vec4 {
  auto norm = this->norm();
  if (norm == 0)
    return *this;

  return (*this) / norm;
}

template <typename T> auto Vec4<T>::square_norm() const -> T {
  return x * x + y * y + z * z + w * w;
}

template <typename T> auto Vec4<T>::norm() const -> T {
  return std::sqrt(this->square_norm());
}

template <typename T>
template <typename U>
auto Vec4<T>::cast() const -> Vec4<U> {
  return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z),
          static_cast<U>(w)};
}

template <typename T> auto Vec4<T>::From(const T *data) -> Vec4 {
  return {data[0], data[1], data[2], data[3]};
}

template <typename T> auto operator*(T lhs, const Vec4<T> &rhs) -> Vec4<T> {
  return rhs * lhs;
}

} // namespace RB