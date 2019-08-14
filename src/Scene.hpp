#pragma once
#include "Mesh.hpp"
#include "Renderer.hpp"
#include <memory>

class Scene {
public:
  Scene() {}
  void add(std::shared_ptr<Mesh> mesh) { meshes.push_back(mesh); };
  friend class Renderer;

protected:
  std::vector<std::shared_ptr<Mesh>> meshes;
};
