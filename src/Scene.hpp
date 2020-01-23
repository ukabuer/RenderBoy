#pragma once
#include "Camera.hpp"
#include "Light.hpp"
#include "Material/AbstractMaterial.hpp"
#include "Mesh.hpp"
#include <memory>

struct Scene {
  std::vector<Mesh *> meshes = {};
  std::vector<Light> lights = {};
  Eigen::Vector3f background = {0.f, 0.f, 0.f};
};
