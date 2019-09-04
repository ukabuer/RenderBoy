#pragma once
#include "Camera.hpp"

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(float fovy, uint32_t width, uint32_t height, float near,
                    float far);

  ~PerspectiveCamera() {}

  Eigen::Matrix4f getViewMatrix() const override;
  Eigen::Matrix4f getProjectionMatrix() const override;

private:
  mutable Eigen::Matrix4f viewMatrix;
  mutable Eigen::Matrix4f projectionMatrix;
};
