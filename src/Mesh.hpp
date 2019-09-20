#pragma once

#include "Geometry.hpp"
#include "Material/Material.hpp"

struct Mesh {
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;

  static auto From(std::shared_ptr<Geometry> geo,
                   std::shared_ptr<Material> material) -> Mesh {
    return {std::move(geo), std::move(material)};
  }
};
