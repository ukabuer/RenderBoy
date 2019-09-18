#pragma once

#include "Light/PointLight.hpp"
#include "Primitives.hpp"

class Material {
public:
  Material() = default;

  virtual Eigen::Vector3f
  getColor(const Point &point,
           const std::vector<std::shared_ptr<PointLight>> &lights,
	  const Eigen::Vector3f &view) const = 0;

  virtual ~Material() = default;
};
