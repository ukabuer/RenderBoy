#include "Camera/PerspectiveCamera.hpp"
#include "Geometry.hpp"
#include <Eigen/Dense>
#include <cmath>

using namespace std;
using namespace Eigen;

static Matrix4f calculateProjectionMatrix(float fov, float aspect, float zNear,
                                          float zFar) {
  Matrix4f result = Matrix4f::Zero();

  const auto radf = fov * PI / 180.f;
  const auto tanHalfFovy = tan(radf / 2.0f);

  result(0, 0) = 1.0f / (aspect * tanHalfFovy);
  result(1, 1) = 1.0f / tanHalfFovy;
  result(2, 2) = -(zFar + zNear) / (zFar - zNear);
  result(3, 2) = -1.0f;
  result(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);

  return result;
}

PerspectiveCamera::PerspectiveCamera(float fov, uint32_t width, uint32_t height,
                                     float zNear, float zFar)
    : Camera(width, height), fov(fov), zNear(zNear), zFar(zFar) {

  this->projectionMatrix = calculateProjectionMatrix(
      fov, static_cast<float>(width) / height, zNear, zFar);
}

void PerspectiveCamera::setFOV(float fov) {
  if (this->fov == fov) {
    return;
  }

  this->fov = fov;
  this->projectionChanged = true;
}

void PerspectiveCamera::setZNear(float zNear) {
  if (this->zNear == zNear) {
    return;
  }

  this->zNear = zNear;
  this->projectionChanged = true;
}

void PerspectiveCamera::setZFar(float zFar) {
  if (this->zFar == zFar) {
    return;
  }

  this->zFar = zFar;
  this->projectionChanged = true;
}

Matrix4f PerspectiveCamera::getProjectionMatrix() const {
  if (this->projectionChanged) {
    const auto aspect = static_cast<float>(width) / height;
    this->projectionMatrix =
        calculateProjectionMatrix(this->fov, aspect, this->zNear, this->zFar);
    this->projectionChanged = false;
  }
  return this->projectionMatrix;
}
