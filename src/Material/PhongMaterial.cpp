#include "Material/PhongMaterial.hpp"

using namespace std;
using namespace Eigen;

auto PhongMaterial::getColor(const Point &point,
                             const vector<shared_ptr<PointLight>> &pointLights,
                             const Camera &camera) const -> Vector3f {
  Vector3f result(0.0f, 0.0f, 0.0f);

  const Vector3f V = (camera.getPosition() - point.position).normalized();
  for (auto &pointLight : pointLights) {
    const auto N = point.normals;
    const Vector3f L = (pointLight->position - point.position).normalized();
    const Vector3f H = (L + V).normalized();
    const auto lightColor = pointLight->color;

    Vector3f diffuse(0.0f, 0.0f, 0.0f);
    Vector3f specular(0.0f, 0.0f, 0.0f);
    Vector3f ambient = ambientColor * 0.5f;

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