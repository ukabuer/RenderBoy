#pragma once

#include "Material/Texture.hpp"
#include "Mesh.hpp"
#include <memory>
#include <string>

struct Model {
  explicit Model(const std::string &path);

  std::string baseDir;
  std::vector<std::shared_ptr<Mesh>> meshes;
  std::vector<std::shared_ptr<Texture>> textures;
  std::vector<std::shared_ptr<Material>> materials;
};
