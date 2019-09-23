#pragma once
#include "Primitives.hpp"
#include <array>

inline auto SampleDepthMaterial(const Point &point) -> Eigen::Vector3f {
  const auto gray = (point.z + 1.0f) / 2.0f;

  return {gray, gray, gray};
}
