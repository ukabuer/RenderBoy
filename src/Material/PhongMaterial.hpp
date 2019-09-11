#pragma once

#include "Material/Material.hpp"
#include "Material/Texture.hpp"
#include "Light/PointLight.hpp"
#include <array>
#include <vector>
#include <memory>

inline Eigen::Vector3f reflect(const Eigen::Vector3f &n,
                        const Eigen::Vector3f &l) {
  return (n * (n.dot(l) * 2.0f) - l).normalized();
}

// TODO
class PhongMaterial : public Material {
public:
  PhongMaterial() = default;

  PhongMaterial(std::vector<std::shared_ptr<Texture>> textures)
      : textures(std::move(textures)) {}

  std::array<unsigned char, 4>
  getColor(const Point &point,
           const std::vector<std::shared_ptr<PointLight>> &pointLights) const override {
    const Eigen::Vector3f lightDir = (pointLights[0]->getPosition() - point.position).normalized();
    const Eigen::Vector3f reflected = reflect(point.normals, lightDir);
    const auto spec = pow(std::max(reflected[2], 0.0f), 50.0f);
    const auto diff = std::max(0.f, point.normals.dot(lightDir));

    std::array<unsigned char, 4> color =
        textures[0]->getColor(point.u, 1.0f - point.v);
    for (int i = 0; i < 3; i++) {
      color[i] = std::min<unsigned char>(5 + color[i] * (diff + .6 * spec), 255);
    }
    return color;
  }

private:
  std::vector<std::shared_ptr<Texture>> textures;
};