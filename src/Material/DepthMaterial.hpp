#pragma once

#include "Material.hpp"
#include "Primitives.hpp"
#include <array>

class DepthMaterial : public Material {
  Eigen::Vector3f
  getColor(const Point &point,
           const std::vector<std::shared_ptr<PointLight>> &lights,
           const Eigen::Vector3f &view) const override {
    const auto gray = (point.z + 1.0f) / 2.0f;

    return Eigen::Vector3f{gray, gray, gray};
  }
};