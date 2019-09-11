#pragma once

#include "Primitives.hpp"
#include "Light/PointLight.hpp"

class Material {
public:
  virtual std::array<unsigned char, 4>
  getColor(const Point &point,
           const std::vector<std::shared_ptr<PointLight>> &lights) const = 0;

  virtual ~Material() = default;
};
