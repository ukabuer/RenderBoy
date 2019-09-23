#pragma once
#include "Light.hpp"
#include "Mesh.hpp"

struct Scene {
  std::vector<Mesh> meshes = {};
  std::vector<Light> lights = {};
  Eigen::Vector3f background = {0.f, 0.f, 0.f};
};
