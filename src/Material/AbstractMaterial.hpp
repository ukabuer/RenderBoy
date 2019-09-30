#pragma once
#include "Camera.hpp"
#include "Light.hpp"
#include "Primitives.hpp"
#include <vector>

class Material {
public:
  Material() = default;
  virtual ~Material() = default;
 
  virtual auto sample(const Vertex &vertex, const std::vector<Light> &lights,
              const Camera &camera) const -> Eigen::Vector3f = 0;

private:
  enum class Type : uint8_t {
    Phong,
    Nomral,
    Depth,
    Gouraud,
    None,
  };

  Type type = Type::None;
};
