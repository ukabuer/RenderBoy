#pragma once
#include "Camera/Camera.hpp"
#include "Light/PointLight.hpp"
#include "Primitives.hpp"
#include <vector>

class Material {
public:
  Material() = default;

  virtual auto getColor(const Point &point,
                        const std::vector<std::shared_ptr<PointLight>> &lights,
                        const Camera &camera) const -> Eigen::Vector3f = 0;

  virtual ~Material() = default;
};
