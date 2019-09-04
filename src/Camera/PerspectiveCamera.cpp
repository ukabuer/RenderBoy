#include "Camera/PerspectiveCamera.hpp"
#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

static inline Matrix4f calculateViewMatrix(const Vector3f &pos, const Vector3f &target,
                                    const Vector3f &up) {
  Matrix4f result;
  auto zAxis = (pos - target).normalized();
  auto xAxis = up.cross(zAxis).normalized();
  auto yAxis = zAxis.cross(xAxis);
  result << xAxis[0], yAxis[0], zAxis[0], pos[0], xAxis[1], yAxis[1], zAxis[1],
      pos[1], xAxis[2], yAxis[2], zAxis[2], pos[2], 0, 0, 0, 1;
  return result;
}

PerspectiveCamera::PerspectiveCamera(float fovy, uint32_t width,
                                     uint32_t height, float zNear, float zFar)
    : Camera(width, height) {
  float radf = fovy;

  float tanHalfFovy = tan(radf / 2.0f);
  Matrix4f res = Matrix4f::Zero();

  float aspect = (float)width / height;
  res(0, 0) = 1.0f / (aspect * tanHalfFovy);
  res(1, 1) = 1.0f / (tanHalfFovy);
  res(2, 2) = -(zFar + zNear) / (zFar - zNear);
  res(3, 2) = -1.0f;
  res(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
  this->projectionMatrix = res;

  this->target = Vector3f(0, 0, 0);
  this->up = Vector3f(0, 1, 0);
}

Matrix4f PerspectiveCamera::getViewMatrix() const {
  if (changed) {
    this->viewMatrix = calculateViewMatrix(position, target, up);
    changed = false;
  }
  return this->viewMatrix;
}

Matrix4f PerspectiveCamera::getProjectionMatrix() const {
  if (changed) {
    changed = false;
  }
  return this->projectionMatrix;
}
