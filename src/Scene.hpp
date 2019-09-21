#pragma once
#include "Light.hpp"
#include "Mesh.hpp"

struct Scene {
  std::vector<Mesh> meshes = {};
  std::vector<Light> lights = {};
};
