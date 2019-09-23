#pragma once
#include "Primitives.hpp"
#include <vector>

inline auto SampleNormalMaterial(const Vertex &v) -> Eigen::Vector3f {
  return {(v.normals[0] + 1.0f) / 2.0f, (v.normals[1] + 1.0f) / 2.0f,
          (v.normals[2] + 1.0f) / 2.0f};
}
