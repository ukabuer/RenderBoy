#pragma once

#include "Light/AmbientLight.hpp"
#include "Light/PointLight.hpp"
#include "Mesh.hpp"
#include <memory>

struct Scene {
  std::vector<std::shared_ptr<Mesh>> meshes = {};
  std::vector<std::shared_ptr<AmbientLight>> ambientLights = {};
  std::vector<std::shared_ptr<PointLight>> pointLights = {};
};
