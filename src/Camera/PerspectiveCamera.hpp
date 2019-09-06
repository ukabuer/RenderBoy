#pragma once
#include "Camera.hpp"

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(float fov, uint32_t width, uint32_t height, float near,
                    float far);

  void setFOV(float fov);
  float getFOV() const { return this->fov; }

  void setZNear(float zNear);
  float getZNear() const { return this->zNear; }

  void setZFar(float zFar);
  float getZFar() const { return this->zFar; }

  Eigen::Matrix4f getProjectionMatrix() const override;

private:
  float fov;
  float zNear;
  float zFar;
};
