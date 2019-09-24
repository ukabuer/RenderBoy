#pragma once
#include "Camera.hpp"
#include "Light.hpp"
#include "Material/PhongMaterial.hpp"
#include "Primitives.hpp"

struct GouraudMaterialData {
  Eigen::Vector3f color = {1.f, 0.6f, 0.f};
};

inline auto SampleGouraudMaterial(const Vertex &v,
                                  const std::vector<Light> &lights,
                                  const Camera &camera,
                                  const GouraudMaterialData &data) {
  Eigen::Vector3f color = Eigen::Vector3f::Zero();

  for (auto &light : lights) {
    if (light.type == Light::Type::Ambient) {
      color += light.intensity * data.color;
    } else if (light.type == Light::Type::Point) {
      auto lightDir = (light.position - v.position).normalized();
      auto intensity = std::max(0.f, v.normals.dot(lightDir));
      if (intensity > .85) {
        intensity = 1.f;
      } else if (intensity > .6f) {
        intensity = .8f;
      } else if (intensity > .4f) {
        intensity = .6f;
      } else if (intensity > .3f) {
        intensity = .45f;
      } else if (intensity > .15f) {
        intensity = .30f;
      } else {
        intensity = 0.f;
      }
      color += data.color * intensity;
    }
  }

  return color;
}