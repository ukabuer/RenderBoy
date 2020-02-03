#pragma once
#include <RenderBoy/Mesh.hpp>
#include <memory>
#include <string>

namespace RB {

struct Model {
  std::vector<Mesh> meshes;
  BoundingBox box;
};

} // namespace RB
