#pragma once
#include "Eigen/Core"
#include <cstdint>

class Camera {
public:
  Camera(int w, int h): width(w), height(h) {}
  virtual Eigen::Matrix4f getViewMatrix() const = 0;
  virtual Eigen::Matrix4f getProjectionMatrix() const = 0;
  inline int getWidth() const { return width; }
  inline int getHeight() const { return height; }
  ~Camera() {}

protected:
  int width;
  int height;
  Eigen::Vector3f position;
  Eigen::Vector3f target;
  Eigen::Vector3f up;
  Eigen::Matrix4f matrix;
};