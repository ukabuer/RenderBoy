#include "Camera/Camera.hpp"
#include <Eigen/Geometry>

using namespace Eigen;

static Matrix4f calculateViewMatrix(const Vector3f &pos,
                                           const Vector3f &target,
                                           const Vector3f &up) {
  Matrix4f result = Matrix4f::Zero();
  auto zAxis = (pos - target).normalized();
  auto xAxis = up.cross(zAxis).normalized();
  auto yAxis = zAxis.cross(xAxis);
  result << xAxis[0], yAxis[0], zAxis[0], pos[0], xAxis[1], yAxis[1], zAxis[1],
      pos[1], xAxis[2], yAxis[2], zAxis[2], pos[2], 0, 0, 0, 1;
  return result;
}

Camera::Camera(uint32_t w, uint32_t h)
      : width(w), height(h), position(0, 0, 1), target(0, 0, 0), up(0, 1, 0),
        viewChanged(false), projectionChanged(false), frame(w, h) {
        this->viewMatrix =
            calculateViewMatrix(this->position, this->target, this->up);

}

void Camera::setPosition(float x, float y, float z) {
  if (x == this->position[0] && y == this->position[1] &&
      z == this->position[2]) {
    return;
  }

  this->position << x, y, z;
  this->viewChanged = true;
}

void Camera::setPosition(const Eigen::Vector3f &v) {
  if (v[0] == this->position[0] && v[1] == this->position[1] &&
      v[2] == this->position[2]) {
    return;
  }

  this->position << v;
  this->viewChanged = true;
}

void Camera::setUp(float x, float y, float z) {
  if (x == this->up[0] && y == this->up[1] && z == this->up[2]) {
    return;
  }

  this->up << x, y, z;
  this->viewChanged = true;
 }

void Camera::setUp(const Eigen::Vector3f &v) {
   if (v[0] == this->up[0] && v[1] == this->up[1] &&
       v[2] == this->up[2]) {
     return;
   }

   this->up = v;
   this->viewChanged = true;
 }

void Camera::setTarget(float x, float y, float z) {
   if (x == this->target[0] && y == this->target[1] && z == this->target[2]) {
     return;
   }

   this->target << x, y, z;
   this->viewChanged = true;
 }

void Camera::setTarget(const Eigen::Vector3f &v) {
   if (v[0] == this->target[0] && v[1] == this->target[1] && v[2] == this->target[2]) {
     return;
   }

   this->target = v;
   this->viewChanged = true;
 }

void Camera::setWidth(uint32_t w) {
   if (w == this->width) {
     return;
   }

   this->width = w;
   this->viewChanged = true;
 }

void Camera::setHeight(uint32_t h) {
   if (h == this->height) {
     return;
   }

   this->height = h;
   this->viewChanged = true;
 }

Matrix4f Camera::getViewMatrix() const {
   if (this->viewChanged) {
     this->viewMatrix = calculateViewMatrix(position, target, up);
     this->viewChanged = false;
   }
   return this->viewMatrix;
 }
