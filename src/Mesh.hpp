#pragma once
#include "Geometry.hpp"
#include "Material/Texture.hpp"
#include <memory>

class Renderer;

class Mesh {
public:
  Mesh(std::shared_ptr<Geometry> geo, std::vector<Texture *> textures)
      : geometry(geo), textures(std::move(textures)) {}

  inline const Geometry& getGeometry() const {
    return *geometry;
  }

private:
  std::shared_ptr<Geometry> geometry;
  std::vector<Texture *> textures;
};
