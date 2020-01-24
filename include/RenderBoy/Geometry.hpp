#pragma once
#include <Eigen/Core>
#include <RenderBoy/Primitive.hpp>
#include <functional>
#include <memory>
#include <vector>

const auto PI = atanf(1.0f) * 4.0f;

struct Geometry {
  std::vector<Vertex> buffers;
  std::vector<uint32_t> indices;
  uint32_t vertex_count = 0;
  uint32_t index_count = 0;
  // TODO: material
};
