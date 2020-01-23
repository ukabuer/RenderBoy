#pragma once
#include "Camera.hpp"
#include "Model.hpp"

class Renderer {
public:
  virtual ~Renderer() = default;

  virtual auto render(const Model &model, const Camera &camera) -> Frame & = 0;

  Eigen::Vector4f background = {0.0f, 0.0f, 0.0f, 1.0f};
};
