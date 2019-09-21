#include "Material/PhongMaterial.hpp"

using namespace std;
using namespace Eigen;

auto PhongMaterial::getColor(const Point &point,
                             const std::vector<Light> &lights,
                             const Camera &camera) const -> Vector3f {
  Vector3f result(0.0f, 0.0f, 0.0f);

  const Vector3f V = (camera.getPosition() - point.position).normalized();
  for (auto &light : lights) {
    const auto N = point.normals;
    const Vector3f L = (light.position - point.position).normalized();
    const Vector3f H = (L + V).normalized();
    const auto lightColor = light.color;

    Vector3f diffuse(0.0f, 0.0f, 0.0f);
    Vector3f specular(0.0f, 0.0f, 0.0f);
    Vector3f ambient = ambientColor * 0.5f;

    auto diffuseFactor = N.dot(L);
    if (diffuseFactor > 0.f) {
      if (diffuseMap == nullptr) {
        diffuse = diffuseFactor * diffuseColor.cwiseProduct(lightColor);
      } else {
        const auto color = diffuseMap->getColor(point.uv[0], point.uv[1]);
        diffuse = diffuseFactor * color.cwiseProduct(lightColor);
      }
    }

    const auto specularFactor = std::pow(std::max(H.dot(N), 0.0f), shininess);
    if (specularFactor != 0.f) {
      if (specularMap == nullptr) {
        specular = specularFactor * specularColor.cwiseProduct(lightColor);
      } else {
        const auto color = specularMap->getColor(point.uv[0], point.uv[1]);
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