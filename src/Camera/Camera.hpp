#pragma once

#include <Eigen/Core>
#include <cstdint>

class Camera {
public:
  Camera(uint32_t w, uint32_t h);

  virtual ~Camera() = default;

  const Eigen::Vector3f &getPosition() const { return this->position; }
  void setPosition(float x, float y, float z);
  void setPosition(const Eigen::Vector3f &v);

  const Eigen::Vector3f &getUp() const { return this->up; }
  void setUp(float x, float y, float z);
  void setUp(const Eigen::Vector3f &v);

  const Eigen::Vector3f &getTarget() const { return this->target; }
  void setTarget(float x, float y, float z);
  void setTarget(const Eigen::Vector3f &v);

  uint32_t getWidth() const { return this->width; }
  void setWidth(uint32_t w);

  uint32_t getHeight() const { return this->height; }
  void setHeight(uint32_t h);

  virtual Eigen::Matrix4f getViewMatrix() const;

  virtual Eigen::Matrix4f getProjectionMatrix() const = 0;

protected:
  uint32_t width;
  uint32_t height;

  Eigen::Vector3f position;
  Eigen::Vector3f target;
  Eigen::Vector3f up;

  mutable Eigen::Matrix4f viewMatrix;
  mutable Eigen::Matrix4f projectionMatrix;

  mutable bool viewChanged;
  mutable bool projectionChanged;
};
