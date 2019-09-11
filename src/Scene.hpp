#pragma once

#include "Mesh.hpp"
#include "Light/PointLight.hpp"
#include <memory>

class Scene {
public:
  void add(std::shared_ptr<Mesh> mesh) { meshes.push_back(mesh); }
  void add(std::shared_ptr<PointLight> light) { pointLights.push_back(light); }

  const std::vector<std::shared_ptr<Mesh>> &getMeshes() const {
    return meshes;
  }

  const std::vector<std::shared_ptr<PointLight>> &getLights() const {
    return pointLights;
  }

private:
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<PointLight>> pointLights;
};
