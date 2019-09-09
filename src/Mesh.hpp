#pragma once

#include "Geometry.hpp"
#include "Material/Material.hpp"
#include <memory>
#include <utility>

class Mesh {
public:
  Mesh(std::shared_ptr<Geometry> geo, std::shared_ptr<Material> material)
      : geometry(std::move(geo)), material(std::move(material)) {}

  const Geometry &getGeometry() const { return *geometry; }

  const Material &getMaterial() const { return *material; }
  
private:
  std::shared_ptr<Geometry> geometry;
  std::shared_ptr<Material> material;
};
