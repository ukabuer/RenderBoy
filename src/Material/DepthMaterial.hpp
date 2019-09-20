#pragma once
#include "Material.hpp"
#include "Primitives.hpp"
#include <array>

class DepthMaterial : public Material {
public:
  auto getColor(const Point &point,
                const std::vector<std::shared_ptr<PointLight>> &lights,
                const Camera &camera) const -> Eigen::Vector3f override {
    const auto gray = (point.z + 1.0f) / 2.0f;

    return {gray, gray, gray};
  }
};