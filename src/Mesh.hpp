#pragma once
#include "Geometry.hpp"
#include "Material/AbstractMaterial.hpp"

struct Mesh {
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;

  static auto From(std::shared_ptr<Geometry> geometry,
                   std::shared_ptr<Material> material) -> Mesh {
    return {std::move(geometry), std::move(material)};
  }
};
