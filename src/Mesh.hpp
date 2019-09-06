#pragma once

#include "Geometry.hpp"
#include "Material/Texture.hpp"
#include <memory>
#include <utility>

class Mesh {
public:
  Mesh(std::shared_ptr<Geometry> geo, std::vector<Texture *> textures)
      : geometry(std::move(geo)), textures(std::move(textures)) {}

  const Geometry &getGeometry() const { return *geometry; }

  const std::vector<Texture *> &getTextures() const { return textures; }

private:
  std::shared_ptr<Geometry> geometry;
  std::vector<Texture *> textures;
};
