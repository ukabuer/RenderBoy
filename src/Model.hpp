#pragma once

#include "Material/Texture.hpp"
#include "Mesh.hpp"
#include <memory>
#include <string>

struct Model {
  std::string baseDir = "";
  std::vector<Mesh> meshes = {};
  std::vector<std::shared_ptr<Texture>> textures = {};
  std::vector<std::shared_ptr<Material>> materials = {};

  static auto Load(const std::string &filepath) -> Model;
};
