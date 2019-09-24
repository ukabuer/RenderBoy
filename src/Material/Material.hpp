#pragma once
#include "Camera.hpp"
#include "Light.hpp"
#include "Material/GouraudMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include "Primitives.hpp"
#include <vector>

struct Material {
  enum class Type : uint8_t {
    Phong,
    Nomral,
    Depth,
    Gouraud,
    None,
  };

  Type type = Type::None;
  // TODO: union or something better?
  // union {
    PhongMaterialData phong;
    GouraudMaterialData gouraud;
  // };

  auto sample(const Vertex &vertex, const std::vector<Light> &lights,
              const Camera &camera) const -> Eigen::Vector3f;

  static auto Phong() -> Material;
  static auto Normal() -> Material;
  static auto Depth() -> Material;
  static auto Gouraud() -> Material;
};
