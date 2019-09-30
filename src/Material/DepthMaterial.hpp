#pragma once
#include "Material/AbstractMaterial.hpp"
#include "Primitives.hpp"
#include <array>

class DepthMaterial : public Material {
public:
  auto sample(const Vertex &vertex, const std::vector<Light> &lights,
              const Camera &camera) const -> Eigen::Vector3f override {
    const auto gray = (vertex.depth + 1.0f) / 2.0f;

    return {gray, gray, gray};
  }
};
