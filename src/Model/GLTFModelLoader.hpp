#pragma once
#include "Model/IModelLoader.hpp"
#include "tinygltf/tiny_gltf.h"
#include <RenderBoy/Material.hpp>

namespace RB {

class GLTFModelLoader final : public IModelLoader {
public:
  auto load() -> Model & override;

  auto get_extends() const -> BoundingBox override;

private:
  tinygltf::Model gltf_model;
  std::map<uint32_t, Texture> textures;
  Model model;

  void process_node(const tinygltf::Node &node,
                    const Eigen::Matrix4f *parent_transform);

  auto process_mesh(const tinygltf::Mesh &gltf_mesh) -> Mesh;

  auto process_primitive(const tinygltf::Primitive &gltf_primitive) -> Geometry;

  auto process_material(const tinygltf::Material &gltf_material) -> Material;

  void process_sampler(const tinygltf::Sampler &gltf_sampler);

  auto process_texture(const tinygltf::Texture &gltf_texture) -> Texture;
};

} // namespace RB
