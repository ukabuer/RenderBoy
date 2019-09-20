#pragma once
#include "Camera/Camera.hpp"

class PerspectiveCamera : public Camera {
public:
  PerspectiveCamera(float fov, uint32_t width, uint32_t height, float near,
                    float far);

  void setFOV(float fov);
  auto getFOV() const -> float { return this->fov; }

  void setNear(float zNear);
  auto getNear() const -> float { return this->near; }

  void setFar(float zFar);
  auto getFar() const -> float { return this->far; }

  auto getProjectionMatrix() const -> Eigen::Matrix4f override;

private:
  float fov;
  float near;
  float far;
};
