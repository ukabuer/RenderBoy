#pragma once
#include "./Mat.hpp"
#include "./Vec.hpp"

namespace RB {

struct Quaternion {
  Vec4f data;

  Quaternion() = default;

  explicit Quaternion(const Vec4f &_q) noexcept : data{_q} {}

  Quaternion(float x, float y, float z, float w) noexcept : data{x, y, z, w} {}

  Quaternion(const Vec3f &axis, float radian) {
    float norm = axis.norm();
    if (norm != 0) {
      float sina2 = std::sin(0.5f * radian);
      this->data[0] = sina2 * axis[0] / norm;
      this->data[1] = sina2 * axis[1] / norm;
      this->data[2] = sina2 * axis[2] / norm;
      this->data[3] = std::cos(0.5f * radian);
    }
  }

  Quaternion &operator=(const Quaternion &rhs) {
    if (&rhs == this) {
      return *this;
    }

    data = rhs.data;
    return *this;
  }

  bool operator==(const Quaternion &rhs) const { return data == rhs.data; }

  bool operator!=(const Quaternion &rhs) const { return !(*this == rhs); }

  Quaternion operator*(const Quaternion &rhs) const {
    Quaternion out;
    out.data.x = +this->data.x * rhs.data.w + this->data.y * rhs.data.z -
                 this->data.z * rhs.data.y + this->data.w * rhs.data.x;
    out.data.y = -this->data.x * rhs.data.z + this->data.y * rhs.data.w +
                 this->data.z * rhs.data.x + this->data.w * rhs.data.y;
    out.data.z = +this->data.x * rhs.data.y - this->data.y * rhs.data.x +
                 this->data.z * rhs.data.w + this->data.w * rhs.data.z;
    out.data.w = -this->data.x * rhs.data.x - this->data.y * rhs.data.y -
                 this->data.z * rhs.data.z + this->data.w * rhs.data.w;
    return out;
  }

  Quaternion &operator*=(const Quaternion &rhs) {
    *this = *this * rhs;
    return *this;
  }

  void GetAxisAngle(Vec3f &axis, float &angle) const {
    float sina2 =
        std::sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    angle = 2.0f * std::atan2(sina2, data[3]);
    float r = (sina2 > 0) ? (1.0f / sina2) : 0;
    axis[0] = r * data[0];
    axis[1] = r * data[1];
    axis[2] = r * data[2];
  }

  Mat4x4f matrix() const {
    Mat4x4f out;
    float yy2 = 2.0f * data[1] * data[1];
    float xy2 = 2.0f * data[0] * data[1];
    float xz2 = 2.0f * data[0] * data[2];
    float yz2 = 2.0f * data[1] * data[2];
    float zz2 = 2.0f * data[2] * data[2];
    float wz2 = 2.0f * data[3] * data[2];
    float wy2 = 2.0f * data[3] * data[1];
    float wx2 = 2.0f * data[3] * data[0];
    float xx2 = 2.0f * data[0] * data[0];
    out.data[0][0] = -yy2 - zz2 + 1.0f;
    out.data[1][0] = xy2 + wz2;
    out.data[2][0] = xz2 - wy2;
    out.data[3][0] = 0;
    out.data[0][1] = xy2 - wz2;
    out.data[1][1] = -xx2 - zz2 + 1.0f;
    out.data[2][1] = yz2 + wx2;
    out.data[3][1] = 0;
    out.data[0][2] = xz2 + wy2;
    out.data[1][2] = yz2 - wx2;
    out.data[2][2] = -xx2 - yy2 + 1.0f;
    out.data[3][2] = 0;
    out.data[0][3] = out.data[1][3] = out.data[2][3] = 0;
    out.data[3][3] = 1;
    return out;
  }

  Vec3f RotateVector(const Vec3f &v) const {
    const Vec3f axis(data.x, data.y, data.z);
    return v + 2.f * cross(axis, cross(axis, v) + data.w * v);
  }

  static Quaternion From(const float *values) {
    return Quaternion{Vec4f::From(values)};
  }
};

inline Quaternion slerp(Quaternion v0, Quaternion v1, float t,
                        bool DoNotNormalize = false) {
  // Only unit quaternions are valid rotations.
  // Normalize to avoid undefined behavior.
  if (!DoNotNormalize) {
    v0.data.normalize();
    v1.data.normalize();
  }

  // Compute the cosine of the angle between the two vectors.
  auto dp = dot(v0.data, v1.data);

  // If the dot product is negative, slerp won't take
  // the shorter path. Note that v1 and -v1 are equivalent when
  // the negation is applied to all four components. Fix by
  // reversing one quaternion.
  if (dp < 0) {
    v1.data = -v1.data;
    dp = -dp;
  }

  const double DOT_THRESHOLD = 0.9995;
  if (dp > DOT_THRESHOLD) {
    // If the inputs are too close for comfort, linearly interpolate
    // and normalize the result.

    Quaternion result = Quaternion{v0.data + t * (v1.data - v0.data)};
    result.data.normalize();
    return result;
  }

  // Since dot is in range [0, DOT_THRESHOLD], acos is safe
  auto theta_0 = std::acos(dp);         // theta_0 = angle between input vectors
  auto theta = theta_0 * t;             // theta = angle between v0 and result
  auto sin_theta = std::sin(theta);     // compute this value only once
  auto sin_theta_0 = std::sin(theta_0); // compute this value only once

  auto s0 =
      cos(theta) -
      dp * sin_theta / sin_theta_0; // == sin(theta_0 - theta) / sin(theta_0)
  auto s1 = sin_theta / sin_theta_0;

  auto v = Quaternion{v0.data * s0 + v1.data * s1};
  if (!DoNotNormalize) {
    v.data.normalize();
  }
  return v;
}

} // namespace RB

#include "./Quaternion.inl"