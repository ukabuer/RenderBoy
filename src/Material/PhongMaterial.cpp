#include "Material/PhongMaterial.hpp"
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;

auto SamplePhongMaterial(const Vertex &v, const std::vector<Light> &lights,
                         const Camera &camera, const PhongMaterialData &data)
    -> Eigen::Vector3f {
  Vector3f result(0.0f, 0.0f, 0.0f);

  Eigen::Vector3f normal = v.normals;
  if (data.normalMap) {
    Matrix3f TBN;
    TBN.col(0) = v.T.normalized();
    TBN.col(1) = v.B.normalized();
    TBN.col(2) = v.normals.normalized();

    Vector3f s = data.normalMap->sample(v.uv[0], v.uv[1]);
    s = s * 2.0f - Vector3f::Ones();
    normal = (TBN * s.normalized()).normalized();
  }

  Vector3f ao = Vector3f::Ones();
  if (data.aoMap) {
    ao = data.aoMap->sample(v.uv[0], v.uv[1]);
  }

  auto diffuseColor = data.diffuseColor;
  if (data.diffuseMap) {
    const auto sampled = data.diffuseMap->sample(v.uv[0], v.uv[1]);
    diffuseColor = diffuseColor.cwiseProduct(sampled);
  }

  auto specularColor = data.specularColor;
  if (data.specularMap) {
    const auto sampled = data.diffuseMap->sample(v.uv[0], v.uv[1]);
    specularColor = specularColor.cwiseProduct(sampled);
  }

  const Vector3f V = (camera.getPosition() - v.position).normalized();
  for (auto &light : lights) {
    if (light.type == Light::Type::Ambient) {
      result += (light.intensity * diffuseColor).cwiseProduct(ao);
    } else if (light.type == Light::Type::Point) {
      const auto N = normal;
      const Vector3f distance = light.position - v.position;
      const Vector3f L = distance.normalized();
      const Vector3f H = (L + V).normalized();
      const auto lightColor = light.color * light.intensity;

      Vector3f diffuse(0.0f, 0.0f, 0.0f);
      Vector3f specular(0.0f, 0.0f, 0.0f);

      const auto diffuseFactor = N.dot(L);
      if (diffuseFactor > 0.f) {
        diffuse = diffuseFactor * diffuseColor.cwiseProduct(lightColor);
      }

      const auto HN = H.dot(N);
      if (HN > 0.f) {
        const auto specularFactor = std::pow(HN, data.shininess);
        specular = specularFactor * specularColor.cwiseProduct(lightColor);
      }

      result += (diffuse + specular) / distance.squaredNorm();
    }
  }

  result[0] = std::min(result[0], 1.0f);
  result[1] = std::min(result[1], 1.0f);
  result[2] = std::min(result[2], 1.0f);

  return result;
}