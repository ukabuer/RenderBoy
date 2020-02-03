#pragma once
#include <cassert>

namespace RB {

template <typename T> auto Vec3<T>::operator+(const Vec3 &rhs) const -> Vec3 {
  return {x + rhs.x, y + rhs.y, z + rhs.z};
}

template <typename T> auto Vec3<T>::operator+=(const Vec3 &rhs) -> Vec3 & {
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

template <typename T> auto Vec3<T>::operator-(const Vec3 &rhs) const -> Vec3 {
  return {x - rhs.x, y - rhs.y, z - rhs.z};
}

template <typename T> auto Vec3<T>::operator-() const -> Vec3 {
  return {-x, -y, -z};
}

template <typename T> auto Vec3<T>::operator-=(const Vec3 &rhs) -> Vec3 & {
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

template <typename T> auto Vec3<T>::operator*(const Vec3 &rhs) const -> Vec3 {
  return {x * rhs.x, y * rhs.y, z * rhs.z};
}

template <typename T> auto Vec3<T>::operator*=(const Vec3 &rhs) -> Vec3 & {
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

template <typename T> auto Vec3<T>::operator*(T rhs) const -> Vec3 {
  return {x * rhs, y * rhs, z * rhs.z};
}

template <typename T> auto Vec3<T>::operator*=(T rhs) -> Vec3 & {
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

template <typename T> auto Vec3<T>::operator/(const Vec3 &rhs) const -> Vec3 {
  assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
  return {x / rhs.x, y / rhs.y, z / rhs.z};
}

template <typename T> auto Vec3<T>::operator/(T rhs) const -> Vec3 {
  assert(rhs != 0);
  return {x / rhs, y / rhs, z / rhs};
}

template <typename T> auto Vec3<T>::operator/=(const Vec3 &rhs) -> Vec3 & {
  assert(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  return *this;
}

template <typename T> auto Vec3<T>::operator/=(T rhs) -> Vec3 & {
  assert(rhs != 0);
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

template <typename T> auto Vec3<T>::operator==(const Vec3 &rhs) const -> bool {
  return x == rhs.x && y == rhs.y && z == rhs.z;
}

template <typename T> auto Vec3<T>::operator!=(const Vec3 &rhs) const -> bool {
  return !(*this == rhs);
}

template <typename T>
auto Vec3<T>::operator[](uint32_t index) const -> const T & {
  assert(index < 3);
  return reinterpret_cast<const T *>(this)[index];
}

template <typename T> auto Vec3<T>::operator[](uint32_t index) -> T & {
  assert(index < 3);
  return reinterpret_cast<T *>(this)[index];
}

template <typename T> auto Vec3<T>::normalize() -> Vec3 & {
  auto norm = this->norm();
  if (norm == 0)
    return;

  (*this) /= norm;
  return *this;
}

template <typename T> auto Vec3<T>::normalized() const -> Vec3 {
  auto norm = this->norm();
  if (norm == 0)
    return *this;

  return (*this) / norm;
}

template <typename T> auto Vec3<T>::square_norm() const -> T {
  return x * x + y * y + z * z;
}

template <typename T> auto Vec3<T>::norm() const -> T {
  return std::sqrt(this->square_norm());
}

template <typename T>
template <typename U>
auto Vec3<T>::cast() const -> Vec3<U> {
  return {static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
}

template <typename T> auto Vec3<T>::From(const T *data) -> Vec3 {
  return {data[0], data[1], data[2]};
}

template <typename T> auto operator*(T lhs, const Vec3<T> &rhs) -> Vec3<T> {
  return rhs * lhs;
}

} // namespace RB