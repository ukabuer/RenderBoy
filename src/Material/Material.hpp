#pragma once
#include "Camera.hpp"
#include "Light.hpp"
#include "Material/PhongMaterial.hpp"
#include "Primitives.hpp"
#include <vector>

struct Material {
  enum class Type : uint8_t {
    Phong,
    Nomral,
    Depth,
    None,
  };

  Type type = Type::None;
  // union {
  PhongMaterialData phong;
  // };

  auto sample(const Vertex &vertex, const std::vector<Light> &lights,
              const Camera &camera) const -> Eigen::Vector3f;

  static auto Phong() -> Material;
  static auto Normal() -> Material;
  static auto Depth() -> Material;
};
