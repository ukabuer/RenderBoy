#pragma once
#include "Geometry.hpp"
#include <memory>

class Renderer;

class Mesh {
public:
  Mesh(std::shared_ptr<Geometry> geo): geometry(geo) {}
  friend class Renderer;

protected:
  std::shared_ptr<Geometry> geometry;
};
