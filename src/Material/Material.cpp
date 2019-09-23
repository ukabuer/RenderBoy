#include "Material/Material.hpp"
#include "Material/DepthMaterial.hpp"
#include "Material/NormalMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include <memory>

using namespace std;
using namespace Eigen;

auto Material::Normal() -> Material {
  auto material = Material();
  material.type = Type::Nomral;
  return material;
}

auto Material::Depth() -> Material {
  auto material = Material();
  material.type = Type::Depth;
  return material;
}

auto Material::Phong() -> Material {
  auto material = Material();
  material.type = Type::Phong;
  auto data = make_unique<PhongMaterialData>();
  return material;
}

auto Material::sample(const Vertex &v, const std::vector<Light> &lights,
                      const Camera &camera) const -> Eigen::Vector3f {
  switch (type) {
  case Type::Depth:
    return SampleDepthMaterial(v);
  case Type::Nomral:
    return SampleNormalMaterial(v);
  case Type::Phong:
    return SamplePhongMaterial(v, lights, camera, phong);
  default:
    return Vector3f::Zero();
  }
}
