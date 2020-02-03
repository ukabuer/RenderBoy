#pragma once

namespace RB {

template <class T> T dot(const Vec2<T> &a, const Vec2<T> &b) {
  return a.x * b.x + a.y * b.y;
}

template <class T> T dot(const Vec3<T> &a, const Vec3<T> &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <class T> T dot(const Vec4<T> &a, const Vec4<T> &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template <class T> auto cross(const Vec3<T> &a, const Vec3<T> &b) -> Vec3<T> {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

template <typename T> T clamp(T val, T min, T max) {
  return val < min ? min : (val > max ? max : val);
}

template <typename T>
auto clamp(const Vec2<T> &a, const Vec2<T> &min, const Vec2<T> &max)
    -> Vec2<T> {
  return {clamp(a.x, min.x, max.x), clamp(a.y, min.y, max.y)};
}

template <typename T>
auto clamp(const Vec3<T> &a, const Vec3<T> &min, const Vec3<T> &max)
    -> Vec3<T> {
  return Vec3<T>(clamp(a.x, min.x, max.x), clamp(a.y, min.y, max.y),
                 clamp(a.z, min.z, max.z));
}

template <typename T>
auto clamp(const Vec4<T> &a, const Vec4<T> &min, const Vec4<T> &max)
    -> Vec4<T> {
  return Vec4<T>(clamp(a.x, min.x, max.x), clamp(a.y, min.y, max.y),
                 clamp(a.z, min.z, max.z), clamp(a.w, min.w, max.w));
}

} // namespace RB