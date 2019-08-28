#pragma once
#include "Mesh.hpp"
#include <memory>
#include <string>

class Model {
public:
  explicit Model(const std::string &path);
  std::vector<std::shared_ptr<Mesh>> meshes;
};
