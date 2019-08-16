#pragma once
#include "Eigen/Core"
#include <cstdint>

class Camera {
public:
  Camera(int w, int h): width(w), height(h) {}
  int width;
  int height;
  virtual Eigen::Matrix4f getViewMatrix() const = 0;
  virtual Eigen::Matrix4f getProjectionMatrix() const = 0;

protected:
  Eigen::Vector3f position;
  Eigen::Vector3f target;
  Eigen::Vector3f up;
  Eigen::Matrix4f matrix;
};