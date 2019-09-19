#pragma once

#include "Geometry.hpp"
#include "Material/Material.hpp"

struct Mesh {
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;

  static Mesh From(std::shared_ptr<Geometry> geo, std::shared_ptr<Material> material) {
    return Mesh {std::move(geo), std::move(material)};
  }
};
