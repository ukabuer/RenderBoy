#include "Camera/OrthographicCamera.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;

static auto calculateProjectionMatrix(float left, float right, float top,
                                      float bottom, float near, float far)
    -> Matrix4f {
  Matrix4f result = Matrix4f::Zero();

  const auto w = 1.0f / (right - left);
  const auto h = 1.0f / (top - bottom);
  const auto p = 1.0f / (far - near);

  const auto x = (right + left) * w;
  const auto y = (top + bottom) * h;
  const auto z = (far + near) * p;

  result(0, 0) = 2.0f * w;
  result(1, 3) = -x;
  result(1, 1) = 2.0f * h;
  result(1, 3) = -y;
  result(2, 2) = -2.0f * p;
  result(2, 3) = -z;
  result(3, 3) = 1.0f;

  return result;
}

OrthographicCamera::OrthographicCamera(uint32_t width, uint32_t height,
                                       float left, float right, float top,
                                       float bottom, float near, float far)
    : Camera(width, height), left(left), right(right), top(top), bottom(bottom),
      near(near), far(far) {}

auto OrthographicCamera::getProjectionMatrix() const -> Matrix4f {
  if (!projectionChanged) {
    return projectionMatrix;
  }

  auto dx = (right - left) / 2.0f;
  auto dy = (top - bottom) / 2.0f;
  auto cx = (right + left) / 2.0f;
  auto cy = (top + bottom) / 2.0f;

  auto left = cx - dx;
  auto right = cx + dx;
  auto top = cy + dy;
  auto bottom = cy - dy;

  auto matrix = calculateProjectionMatrix(left, right, top, bottom, near, far);
  return matrix.inverse();
}
