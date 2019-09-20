#pragma once

#include "Material/Material.hpp"
#include "Primitives.hpp"
#include <vector>

class NormalMaterial : public Material {
public:
  auto getColor(const Point &point,
                const std::vector<std::shared_ptr<PointLight>> &lights,
                const Camera &camera) const -> Eigen::Vector3f override {
    return {(point.normals[0] + 1.0f) / 2.0f, (point.normals[1] + 1.0f) / 2.0f,
            (point.normals[2] + 1.0f) / 2.0f};
  }
};
