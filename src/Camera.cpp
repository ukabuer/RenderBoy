#include "Camera.hpp"
#include "Eigen/src/Core/Matrix.h"
#include "Geometry.hpp"
#include <Eigen/Geometry>

using namespace Eigen;

static auto perspective(float left, float right, float top, float bottom,
                        float near, float far) -> Matrix4f {
  Matrix4f m = Matrix4f::Zero();

  m(0, 0) = (2.f * near) / (right - left);
  m(0, 2) = (right + left) / (right - left);
  m(1, 1) = (2.f * near) / (top - bottom);
  m(1, 2) = (top + bottom) / (top - bottom);
  m(2, 2) = -(far + near) / (far - near);
  m(2, 3) = -(2.f * far * near) / (far - near);
  m(3, 2) = -1.f;

  return m;
}

static auto orthographic(float left, float right, float top, float bottom,
                         float near, float far) -> Matrix4f {
  Matrix4f m = Matrix4f::Zero();

  const auto w = 1.0f / (right - left);
  const auto h = 1.0f / (top - bottom);
  const auto p = 1.0f / (far - near);

  const auto x = (right + left) * w;
  const auto y = (top + bottom) * h;
  const auto z = (far + near) * p;

  m(0, 0) = 2.0f * w;
  m(1, 3) = -x;
  m(1, 1) = 2.0f * h;
  m(1, 3) = -y;
  m(2, 2) = -2.0f * p;
  m(2, 3) = -z;
  m(3, 3) = 1.0f;

  return m;
}

static auto lookAtMatrix(const Vector3f &pos, const Vector3f &target,
                 const Vector3f &up) -> Matrix4f {
  Matrix4f m = Matrix4f::Zero();

  Vector3f zAxis = (pos - target).normalized();
  Vector3f xAxis = up.cross(zAxis).normalized();
  Vector3f yAxis = zAxis.cross(xAxis);

  m(0, 0) = xAxis[0];
  m(0, 1) = yAxis[0];
  m(0, 2) = zAxis[0];
  m(0, 3) = pos[0];
  m(1, 0) = xAxis[1];
  m(1, 1) = yAxis[1];
  m(1, 2) = zAxis[1];
  m(1, 3) = pos[1];
  m(2, 0) = xAxis[2];
  m(2, 1) = yAxis[2];
  m(2, 2) = zAxis[2];
  m(2, 3) = pos[2];
  m(3, 3) = 1.f;

  return m;
}

void Camera::setProjection(float left, float right, float top, float bottom,
                           float near, float far) noexcept {
  this->left = left;
  this->right = right;
  this->top = top;
  this->bottom = bottom;
  this->near = near;
  this->far = far;
  switch (type) {
  case Type::Perspective:
    projection = perspective(left, right, top, bottom, near, far);
    break;
  case Type::Orthographic:
    projection = orthographic(left, right, top, bottom, near, far);
    break;
  default:
    break;
  }
}

void Camera::setProjection(float fov, float near, float far) noexcept {
  assert(type == Type::Perspective);

  auto h = std::tanf(fov * PI / 360.0f) * near;
  auto w = h * frame.aspect;

  this->left = -w;
  this->right = w;
  this->top = h;
  this->bottom = -h;
  this->near = near;
  this->far = far;

  projection = perspective(-w, w, h, -h, near, far);
}

void Camera::lookAt(const Eigen::Vector3f &position, const Eigen::Vector3f &target, const Eigen::Vector3f &up) noexcept {
  this->position = position;
  this->target = target;
  this->up = up;

  this->view = lookAtMatrix(position, target, up);
}

void Camera::setPosition(float x, float y, float z) {
  this->position = {x, y, z};
  this->view = lookAtMatrix(position, target, up);
}

void Camera::setPosition(const Vector3f &position) {
  this->position = position;
  this->view = lookAtMatrix(position, target, up);
}

void Camera::setTarget(float x, float y, float z) {
  this->target = {x, y, z};
  this->view = lookAtMatrix(position, target, up);
}

void Camera::setTarget(const Vector3f &target) {
  this->target = target;
  this->view = lookAtMatrix(position, target, up);
}

void Camera::setUp(float x, float y, float z) {
  this->up = {x, y, z};
  this->view = lookAtMatrix(position, target, up);
}

void Camera::setUp(const Vector3f &up) {
  this->up = up;
  this->view = lookAtMatrix(position, target, up);
}

void Camera::clearFrame(const Vector3f &color, float z) {
  for (auto i = 0ul; i < frame.size; i++) {
    this->setZ(i, -FLT_MAX);
    this->setColor(i, color);
  }
}