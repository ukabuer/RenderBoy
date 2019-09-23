#pragma once
#include "Primitives.hpp"
#include <array>

inline auto SampleDepthMaterial(const Vertex &vertex) -> Eigen::Vector3f {
  const auto gray = (vertex.depth + 1.0f) / 2.0f;

  return {gray, gray, gray};
}
