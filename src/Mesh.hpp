#pragma once

#include "Geometry.hpp"
#include "Material/Material.hpp"

class Mesh {
public:
  Mesh() = default;
  Mesh(std::shared_ptr<Geometry> geo, std::shared_ptr<Material> material)
      : geometry(std::move(geo)), material(std::move(material)) {}

  Geometry &getGeometry() const { return *geometry; }

  Material &getMaterial() const { return *material; }

private:
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;
};
