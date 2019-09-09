#pragma once

#include "Primitives.hpp"

class Material {
public:
  virtual std::array<unsigned char, 4> getColor(const Point &point) const = 0;

  virtual ~Material() = default;
};
