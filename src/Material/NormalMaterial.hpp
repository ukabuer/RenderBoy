#pragma once

#include "Material/Material.hpp"
#include "Primitives.hpp"
#include <array>

class NormalMaterial : public Material {
  std::array<unsigned char, 4> getColor(const Point &point) const override {
    std::array<unsigned char, 4> color = {0, 0, 0, 255};

    color[0] = static_cast<unsigned char>(point.normals[0] * 255);
    color[1] = static_cast<unsigned char>(point.normals[1] * 255);
    color[2] = static_cast<unsigned char>(point.normals[2] * 255);

    return color;
  }
};