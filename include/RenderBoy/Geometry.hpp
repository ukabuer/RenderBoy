#pragma once
#include <Eigen/Core>
#include <RenderBoy/Primitive.hpp>
#include <cfloat>
#include <functional>
#include <memory>
#include <vector>

namespace RB {

const auto PI = atanf(1.0f) * 4.0f;

struct BoundingBox {
  Eigen::Vector3f min = {FLT_MAX, FLT_MAX, FLT_MAX};
  Eigen::Vector3f max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
};

struct Geometry {
  std::vector<Vertex> buffers;
  std::vector<uint32_t> indices;
  uint32_t vertex_count = 0;
  uint32_t index_count = 0;
  BoundingBox box;
  // TODO: material
};

} // namespace RB
