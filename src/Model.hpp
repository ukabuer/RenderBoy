#pragma once
#include "Material/Texture.hpp"
#include "Mesh.hpp"
#include "Scene.hpp"
#include <memory>
#include <string>

class Model {

public:
  std::string dir;
  std::vector<Mesh> meshes;
  std::vector<std::vector<unsigned char>> buffers;
  std::vector<std::vector<uint32_t>> indices;
  std::vector<Texture> textures;
  //  std::vector<std::shared_ptr<Material>> materials = {};

  auto loadGLTF(const std::string &filepath) -> const Model &;
};
