#pragma once

#include "Material/Material.hpp"
#include "Material/Texture.hpp"
#include <array>
#include <vector>
#include <memory>

// TODO
class PhongMaterial : public Material {
public:
  PhongMaterial() = default;

  PhongMaterial(std::vector<std::shared_ptr<Texture>> textures)
      : textures(std::move(textures)) {}

  std::array<unsigned char, 4> getColor(const Point &point) const override {
    return textures[0]->getColor(point.u, 1.0f - point.v);
  }

private:
  std::vector<std::shared_ptr<Texture>> textures;
};