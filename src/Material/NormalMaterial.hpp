#pragma once
#include "Material/AbstractMaterial.hpp"
#include "Material/Texture.hpp"
#include "Primitives.hpp"
#include <memory>
#include <vector>

class NormalMaterial : public Material {
public:
  auto sample(const Vertex &v, const std::vector<Light> &lights,
              const Camera &camera) const -> Eigen::Vector3f override {
    auto normal = v.normals;
    if (normalMap) {
      Eigen::Matrix3f TBN;
      TBN.col(0) = v.T.normalized();
      TBN.col(1) = v.B.normalized();
      TBN.col(2) = v.normals.normalized();

      Eigen::Vector3f s = normalMap->sample(v.uv[0], v.uv[1]);
      s = s * 2.0f - Eigen::Vector3f::Ones();
      normal = (TBN * s.normalized()).normalized();
    }

    return (normal + Eigen::Vector3f::Ones()) / 2.0f;
  }

private:
  std::shared_ptr<Texture> normalMap;
};
