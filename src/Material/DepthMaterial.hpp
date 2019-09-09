#pragma once

#include "Material.hpp"
#include "Primitives.hpp"
#include <array>

class DepthMaterial : public Material {
  std::array<unsigned char, 4> getColor(const Point &point) const override {
    const auto gray = static_cast<unsigned char>((point.z + 1.0f) / 2 * 255.0f);

    return std::array<unsigned char, 4>{gray, gray, gray, 255};
  }
};