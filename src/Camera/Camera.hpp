#pragma once

#include "Eigen/Core"
#include <cstdint>

class Camera {
public:
  Camera(uint32_t w, uint32_t h)
      : width(w), height(h), position(0, 0, 1), up(0, 1, 0), target(0, 0, 0) {}

  virtual ~Camera() {};

  inline void setPosition(float x, float y, float z) {
    position << x, y, z;
    changed = true;
  }

  inline void setUp(float x, float y, float z) {
    up << x, y, z;
    changed = true;
  }

  inline void setTarget(float x, float y, float z) {
    target << x, y, z;
    changed = true;
  }

  inline uint32_t getWidth() const { return width; }

  inline uint32_t getHeight() const { return height; }

  virtual Eigen::Matrix4f getViewMatrix() const = 0;

  virtual Eigen::Matrix4f getProjectionMatrix() const = 0;

protected:
  uint32_t width;
  uint32_t height;

  Eigen::Vector3f position;
  Eigen::Vector3f target;
  Eigen::Vector3f up;

  mutable Eigen::Matrix4f matrix;

  mutable bool changed;
};
