#pragma once

#include "Camera/Camera.hpp"
#include "Frame.hpp"
#include "Scene.hpp"

class Renderer {
public:
  virtual ~Renderer() = default;

  virtual Frame render(const Scene &scene, const Camera &camera) = 0;
};
