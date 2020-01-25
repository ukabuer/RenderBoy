#pragma once
#include <RenderBoy/Mesh.hpp>
#include <memory>
#include <string>

namespace RB {

struct Model {
  std::vector<Mesh> meshes;
};

} // namespace RB
