#include "Material/PhongMaterial.hpp"
#include "iostream"
using namespace std;
using namespace Eigen;

auto SamplePhongMaterial(const Point &point, const std::vector<Light> &lights,
                         const Camera &camera, const PhongMaterialData &data)
    -> Eigen::Vector3f {
  Vector3f result(0.0f, 0.0f, 0.0f);

  auto diffuseColor = data.diffuseColor;
  if (data.diffuseMap) {
    const auto sampled = data.diffuseMap->sample(point.uv[0], point.uv[1]);
    diffuseColor = diffuseColor.cwiseProduct(sampled);
  }

  auto specularColor = data.specularColor;
  if (data.specularMap) {
    const auto sampled = data.diffuseMap->sample(point.uv[0], point.uv[1]);
    specularColor = specularColor.cwiseProduct(sampled);
  }

  const Vector3f V = (camera.getPosition() - point.position).normalized();
  for (auto &light : lights) {
    if (light.type == Light::Type::Ambient) {
      result += light.intensity * diffuseColor;
    } else if (light.type == Light::Type::Point) {
      const auto N = point.normals;
      const Vector3f L = (light.position - point.position).normalized();
      const Vector3f H = (L + V).normalized();
      const auto lightColor = light.color;

      Vector3f diffuse(0.0f, 0.0f, 0.0f);
      Vector3f specular(0.0f, 0.0f, 0.0f);

      const auto diffuseFactor = N.dot(L);
      if (diffuseFactor > 0.f) {
        diffuse = diffuseFactor * diffuseColor.cwiseProduct(light.color);
      }

      const auto HN = H.dot(N);
      if (HN != 0.f) {
        const auto specularFactor = std::pow(HN, data.shininess);
        specular = specularFactor * specularColor.cwiseProduct(light.color);
      }
      result += diffuse + specular;
    }
  }

  result[0] = std::min(result[0], 1.0f);
  result[1] = std::min(result[1], 1.0f);
  result[2] = std::min(result[2], 1.0f);

  return result;
}