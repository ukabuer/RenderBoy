#pragma once
#include "Light.hpp"
#include "Primitives.hpp"
#include "Camera.hpp"
#include "Material/Texture.hpp"
#include <Eigen/Core>
#include <memory>
#include <vector>

struct PhongMaterialData {
  Eigen::Vector3f diffuseColor;
  Eigen::Vector3f specularColor;
  Eigen::Vector3f ambientColor;
  float shininess;

  std::shared_ptr<Texture> ambientMap;
  std::shared_ptr<Texture> diffuseMap;
  std::shared_ptr<Texture> specularMap;
  std::shared_ptr<Texture> bumpMap;
};

auto SamplePhongMaterial(const Point &point, const std::vector<Light> &lights,
                         const Camera &camera, const PhongMaterialData &data) -> Eigen::Vector3f;
