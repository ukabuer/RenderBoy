#pragma once

#include "./Vec2.hpp"
#include "./Vec3.hpp"
#include "./Vec4.hpp"

namespace RB {

template <class T> T dot(const Vec2<T> &a, const Vec2<T> &b);

template <class T> T dot(const Vec3<T> &a, const Vec3<T> &b);

template <class T> T dot(const Vec4<T> &a, const Vec4<T> &b);

template <class T> auto cross(const Vec3<T> &a, const Vec3<T> &b) -> Vec3<T>;

template <typename T> T clamp(T val, T min, T max);

template <class T>
auto clamp(const Vec2<T> &a, const Vec2<T> &min, const Vec2<T> &max) -> Vec2<T>;

template <class T>
auto clamp(const Vec3<T> &a, const Vec3<T> &min, const Vec3<T> &max) -> Vec3<T>;

template <class T>
auto clamp(const Vec4<T> &a, const Vec4<T> &min, const Vec4<T> &max) -> Vec4<T>;

} // namespace RB

#include "./Vec.inl"