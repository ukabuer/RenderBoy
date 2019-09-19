#pragma once

#include "Light/PointLight.hpp"
#include "Material/Material.hpp"
#include "Material/Texture.hpp"
#include <memory>
#include <vector>

inline Eigen::Vector3f reflect(const Eigen::Vector3f &N,
                               const Eigen::Vector3f &I) {
  return I - 2.0f * N.dot(I) * N;
}

// TODO
class PhongMaterial : public Material {
public:
  Eigen::Vector3f
  getColor(const Point &point,
           const std::vector<std::shared_ptr<PointLight>> &pointLights,
           const Eigen::Vector3f &view) const override {
    Eigen::Vector3f result(0.0f, 0.0f, 0.0f);

    const Eigen::Vector3f V = (view - point.position).normalized();
    for (auto &pointLight : pointLights) {
      const auto N = point.normals;
      const Eigen::Vector3f L =
          (pointLight->position - point.position).normalized();
      const Eigen::Vector3f H = (L + V).normalized();
      const auto lightColor = pointLight->color;

      Eigen::Vector3f diffuse(0.0f, 0.0f, 0.0f);
      Eigen::Vector3f specular(0.0f, 0.0f, 0.0f);
      Eigen::Vector3f ambient = ambientColor * 0.5f;

      auto diffuseFactor = N.dot(L);
      if (diffuseFactor > 0.f) {
        if (diffuseMap == nullptr) {
          diffuse = diffuseFactor * diffuseColor.cwiseProduct(lightColor);
        } else {
          const auto color = diffuseMap->getColor(point.u, point.v);
          diffuse = diffuseFactor * color.cwiseProduct(lightColor);
        }
      }

      const auto specularFactor = std::pow(std::max(H.dot(N), 0.0f), shininess);
      if (specularFactor != 0.f) {
        if (specularMap == nullptr) {
          specular = specularFactor * specularColor.cwiseProduct(lightColor);
        } else {
          const auto color = specularMap->getColor(point.u, 1.0f - point.v);
          specular = diffuseFactor * color.cwiseProduct(lightColor);
        }
      }

      result += diffuse + specular + ambient;
    }

    result[0] = std::min(result[0], 1.0f);
    result[1] = std::min(result[1], 1.0f);
    result[2] = std::min(result[2], 1.0f);

    return result;
  }

  Eigen::Vector3f diffuseColor;
  Eigen::Vector3f specularColor;
  Eigen::Vector3f ambientColor;
  float shininess;

  std::shared_ptr<Texture> ambientMap;
  std::shared_ptr<Texture> diffuseMap;
  std::shared_ptr<Texture> specularMap;
  std::shared_ptr<Texture> bumpMap;
};
