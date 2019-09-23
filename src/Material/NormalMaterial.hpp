#pragma once
#include "Primitives.hpp"
#include <vector>

inline auto SampleNormalMaterial(const Point &point) -> Eigen::Vector3f {
  return {(point.normals[0] + 1.0f) / 2.0f, (point.normals[1] + 1.0f) / 2.0f,
          (point.normals[2] + 1.0f) / 2.0f};
}
