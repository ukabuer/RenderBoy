#pragma once
#include <cstdint>
#include "Scene.hpp"
#include "Camera/Camera.hpp"
#include "Frame.hpp"

class Renderer {
public:
  Renderer() {}
  Frame render(const Scene &scene, const Camera &camera);
  friend class Scene;
};
