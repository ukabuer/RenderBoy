#pragma once
#include "Frame.hpp"
#include <Eigen/Core>
#include <cfloat>
#include <cstdint>
#include <iostream>

class Camera {
public:
  Camera(uint32_t w, uint32_t h);

  virtual ~Camera() = default;

  auto getPosition() const -> const Eigen::Vector3f & { return this->position; }
  void setPosition(float x, float y, float z);
  void setPosition(const Eigen::Vector3f &v);

  auto getUp() const -> const Eigen::Vector3f & { return this->up; }
  void setUp(float x, float y, float z);
  void setUp(const Eigen::Vector3f &v);

  auto getTarget() const -> const Eigen::Vector3f & { return this->target; }
  void setTarget(float x, float y, float z);
  void setTarget(const Eigen::Vector3f &v);

  auto getWidth() const -> uint32_t { return this->width; }
  void setWidth(uint32_t w);

  auto getHeight() const -> uint32_t { return this->height; }
  void setHeight(uint32_t h);

  void setZ(size_t idx, float z) { frame.z[idx] = z; }

  void setZ(uint32_t x, uint32_t y, float z) {
    const auto idx = static_cast<size_t>(x + y * width);
    frame.z[idx] = z;
  }

  void setColor(size_t idx, const Eigen::Vector3f &color) {
    idx = idx << 2;
    frame.colors[idx] = color[0];
    frame.colors[idx + 1] = color[1];
    frame.colors[idx + 2] = color[2];
    frame.colors[idx + 3] = 1.0f;
  }

  void setColor(uint32_t x, uint32_t y, const Eigen::Vector3f &color) {
    const auto idx = static_cast<size_t>(x + y * width);
    this->setColor(idx, color);
  }

  auto getFrame() const -> const Frame & { return frame; }

  void clearFrame(const Eigen::Vector3f &color = {0.0f, 0.0f, 0.0f},
                  float z = -FLT_MAX);

  virtual auto getViewMatrix() const -> Eigen::Matrix4f;

  virtual auto getProjectionMatrix() const -> Eigen::Matrix4f = 0;

protected:
  uint32_t width;
  uint32_t height;

  Eigen::Vector3f position;
  Eigen::Vector3f target;
  Eigen::Vector3f up;

  Frame frame;

  mutable Eigen::Matrix4f viewMatrix;
  mutable Eigen::Matrix4f projectionMatrix;

  mutable bool viewChanged;
  mutable bool projectionChanged;
};
