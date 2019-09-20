#include "Camera/PerspectiveCamera.hpp"
#include "Geometry.hpp"
#include <Eigen/Core>
#include <cmath>

using namespace std;
using namespace Eigen;

static auto calculateProjectionMatrix(float fov, float aspect, float near,
                                      float far) -> Matrix4f {
  Matrix4f result = Matrix4f::Zero();

  const auto radf = fov * PI / 180.f;
  const auto tanHalfFovy = tan(radf / 2.0f);

  result(0, 0) = 1.0f / (aspect * tanHalfFovy);
  result(1, 1) = 1.0f / tanHalfFovy;
  result(2, 2) = -(far + near) / (far - near);
  result(3, 2) = -1.0f;
  result(2, 3) = -(2.0f * far * near) / (far - near);

  return result;
}

PerspectiveCamera::PerspectiveCamera(float fov, uint32_t width, uint32_t height,
                                     float near, float far)
    : Camera(width, height), fov(fov), near(near), far(far) {

  this->projectionMatrix = calculateProjectionMatrix(
      fov, static_cast<float>(width) / height, near, far);
}

void PerspectiveCamera::setFOV(float fov) {
  if (this->fov == fov) {
    return;
  }

  this->fov = fov;
  this->projectionChanged = true;
}

void PerspectiveCamera::setNear(float near) {
  if (this->near == near) {
    return;
  }

  this->near = near;
  this->projectionChanged = true;
}

void PerspectiveCamera::setFar(float far) {
  if (this->far == far) {
    return;
  }

  this->far = far;
  this->projectionChanged = true;
}

auto PerspectiveCamera::getProjectionMatrix() const -> Matrix4f {
  if (this->projectionChanged) {
    const auto aspect = static_cast<float>(width) / height;
    this->projectionMatrix =
        calculateProjectionMatrix(this->fov, aspect, this->near, this->far);
    this->projectionChanged = false;
  }
  return this->projectionMatrix;
}
