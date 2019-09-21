#pragma once
#include "Light.hpp"
#include "Material/Material.hpp"
#include "Material/Texture.hpp"
#include <Eigen/Core>
#include <memory>
#include <vector>

class PhongMaterial : public Material {
public:
  auto getColor(const Point &point, const std::vector<Light> &Lights,
                const Camera &camera) const -> Eigen::Vector3f override;

  Eigen::Vector3f diffuseColor;
  Eigen::Vector3f specularColor;
  Eigen::Vector3f ambientColor;
  float shininess;

  std::shared_ptr<Texture> ambientMap;
  std::shared_ptr<Texture> diffuseMap;
  std::shared_ptr<Texture> specularMap;
  std::shared_ptr<Texture> bumpMap;
};
