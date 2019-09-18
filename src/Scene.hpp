#pragma once

#include "Light/AmbientLight.hpp"
#include "Light/PointLight.hpp"
#include "Mesh.hpp"
#include <memory>

class Scene {
public:
  void add(std::shared_ptr<Mesh> mesh) { meshes.push_back(mesh); }
  void add(std::shared_ptr<PointLight> light) { pointLights.push_back(light); }

  constexpr const std::vector<std::shared_ptr<Mesh>> &getMeshes() const
      noexcept {
    return meshes;
  }

  constexpr const std::vector<std::shared_ptr<AmbientLight>> &
  getAmbientLights() const noexcept {
    return ambientLights;
  }

  constexpr const std::vector<std::shared_ptr<PointLight>> &
  getPointLights() const noexcept {
    return pointLights;
  }

private:
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<AmbientLight>> ambientLights;
  std::vector<std::shared_ptr<PointLight>> pointLights;
};
