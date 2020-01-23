#pragma once
#include "Primitives.hpp"
#include <array>

class DepthMaterial {
public:
  [[nodiscard]] auto sample(float depth) const -> Eigen::Vector4f {
    const auto gray = (depth + 1.0f) / 2.0f;

    return {gray, gray, gray, 1.0f};
  }
};
