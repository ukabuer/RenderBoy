#pragma once
#include "Camera/Camera.hpp"
#include "Frame.hpp"
#include "Scene.hpp"
#include <cstdint>

class Renderer {
public:
  Renderer() {}
  
  virtual ~Renderer() {};

  virtual Frame render(const Scene &scene, const Camera &camera) = 0;
};
