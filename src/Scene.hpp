#pragma once

#include "Mesh.hpp"
#include <memory>

class Scene {
public:
  void add(std::shared_ptr<Mesh> mesh) { meshes.push_back(mesh); }

  const std::vector<std::shared_ptr<Mesh>> &getMeshes() const {
    return meshes;
  }

private:
  std::vector<std::shared_ptr<Mesh>> meshes;
};
