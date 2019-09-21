#pragma once
#include "Camera/Camera.hpp"
#include "Scene.hpp"

class Renderer {
public:
  virtual ~Renderer() = default;

  virtual void render(const Scene &scene, Camera &camera) = 0;
};
