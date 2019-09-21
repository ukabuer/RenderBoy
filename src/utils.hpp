#pragma once
#include "Camera/Camera.hpp"
#include "Scene.hpp"
#include <memory>
#include <string>

struct RenderConfig {
  Scene scene;
  std::unique_ptr<Camera> camera;

  static auto Load(const std::string &filename) -> RenderConfig;
};
