#pragma once
#include "Eigen/Core"
#include <cstdint>

class Camera {
public:
  Camera(uint32_t w, uint32_t h): width(w), height(h) {}
  uint32_t width;
  uint32_t height;
  virtual Eigen::Matrix4f getViewMatrix() const = 0;
  virtual Eigen::Matrix4f getProjectionMatrix() const = 0;

protected:
  Eigen::Vector3f position;
  Eigen::Vector3f target;
  Eigen::Vector3f up;
  Eigen::Matrix4f matrix;
};