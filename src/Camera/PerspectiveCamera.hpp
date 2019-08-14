#pragma once
#include "Camera.hpp"

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(float fovy, uint32_t width, uint32_t height, float near,
                    float far);
  void setPosition(float x, float y, float z);
  Eigen::Matrix4f getViewMatrix() const override;
  Eigen::Matrix4f getProjectionMatrix() const override;

private:
  Eigen::Matrix4f viewMatrix;
  Eigen::Matrix4f projectionMatrix;
};
