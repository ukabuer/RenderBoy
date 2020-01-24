#pragma once
#include "../../include/RenderBoy/Camera.hpp"
#include "Light.hpp"
#include "Material/AbstractMaterial.hpp"
#include "Material/Texture.hpp"
#include "RenderBoy/Primitive.hpp"
#include <Eigen/Core>
#include <memory>
#include <vector>

class PhongMaterial : public Material {
public:
  auto sample(const Vertex &vertex, const std::vector<Light> &lights,
              const Camera &camera) const -> Eigen::Vector3f override;

  void setAmbientColor(const Eigen::Vector3f &color) { ambientColor = color; }

  void setDiffuseColor(const Eigen::Vector3f &color) { diffuseColor = color; }

  void setSpecularColor(const Eigen::Vector3f &color) { specularColor = color; }

  void setShininess(float value) { shininess = value; }

  void setAmbientMap(std::shared_ptr<Texture> map) {
    this->ambientMap = std::move(map);
  }

  void setDiffuseMap(std::shared_ptr<Texture> map) {
    this->diffuseMap = std::move(map);
  }

  void setSpecularMap(std::shared_ptr<Texture> map) {
    this->specularMap = std::move(map);
  }

  void setNormalMap(std::shared_ptr<Texture> map) {
    this->normalMap = std::move(map);
  }

  void setAOMap(std::shared_ptr<Texture> map) { this->aoMap = std::move(map); }

private:
  Eigen::Vector3f ambientColor = Eigen::Vector3f::Ones();
  Eigen::Vector3f diffuseColor = Eigen::Vector3f::Ones();
  Eigen::Vector3f specularColor = Eigen::Vector3f::Ones();
  float shininess = 20.f;

  std::shared_ptr<Texture> ambientMap = nullptr;
  std::shared_ptr<Texture> diffuseMap = nullptr;
  std::shared_ptr<Texture> specularMap = nullptr;
  std::shared_ptr<Texture> normalMap = nullptr;
  std::shared_ptr<Texture> aoMap = nullptr;
};
