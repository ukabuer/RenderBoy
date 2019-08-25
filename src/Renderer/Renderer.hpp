#pragma once
#include "Camera/Camera.hpp"
#include "Frame.hpp"
#include "Scene.hpp"
#include <cstdint>

class Renderer {
public:
  Renderer() {}
  Frame render(const Scene &scene, const Camera &camera);
  friend class Scene;
};
