#pragma once
#include "Camera/Camera.hpp"
#include "Light.hpp"
#include "Primitives.hpp"
#include <vector>

class Material {
public:
  Material() = default;

  virtual auto getColor(const Point &point, const std::vector<Light> &lights,
                        const Camera &camera) const -> Eigen::Vector3f = 0;

  virtual ~Material() = default;
};
