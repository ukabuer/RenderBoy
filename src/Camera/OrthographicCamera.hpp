#pragma once
#include "Camera/Camera.hpp"

class OrthographicCamera : public Camera {
public:
  OrthographicCamera(uint32_t width, uint32_t height, float left, float right,
                     float top, float bottom, float near, float far);

  void setLeft(float left);
  auto getLeft() const -> float { return this->left; }

  void setRight(float right);
  auto getRight() const -> float { return this->right; }

  void setTop(float top);
  auto getTop() const -> float { return this->top; }

  void setBottom(float bottom);
  auto getBottom() const -> float { return this->bottom; }

  void setNear(float near);
  auto getNear() const -> float { return this->near; }

  void setFar(float far);
  auto getFar() const -> float { return this->far; }

  auto getProjectionMatrix() const -> Eigen::Matrix4f override;

private:
  float left;   // Camera frustum left plane
  float right;  // Camera frustum right plane
  float top;    // Camera frustum top plane
  float bottom; // Camera frustum bottom plane
  float near;   // Camera frustum near plane
  float far;    // Camera frustum far plane
};
