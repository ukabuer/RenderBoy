#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Model/GLTFModelLoader.hpp"
#include "RenderBoy/Texture.hpp"
#include <Eigen/Geometry>
#include <RenderBoy/Camera.hpp>
#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace Eigen;

namespace RB {

auto GLTFModelLoader::process_sampler(const tinygltf::Sampler &gltf_sampler)
    -> void {}

auto GLTFModelLoader::process_texture(const tinygltf::Texture &gltf_texture)
    -> Texture {
  auto &gltf_sampler =
      gltf_model.samplers[static_cast<size_t>(gltf_texture.sampler)];

  auto &gltf_image =
      gltf_model.images[static_cast<size_t>(gltf_texture.source)];

  Texture texture{};
  texture.width = static_cast<uint32_t>(gltf_image.width);
  texture.height = static_cast<uint32_t>(gltf_image.height);
  texture.channels = static_cast<uint8_t>(gltf_image.component);
  texture.data = gltf_image.image.data();

  return texture;
}

auto GLTFModelLoader::process_material(const tinygltf::Material &gltf_material)
    -> Material {
  Material material{};

  auto &pbrt = gltf_material.pbrMetallicRoughness;

  if (pbrt.baseColorTexture.index >= 0) {
    auto idx = static_cast<uint32_t>(pbrt.baseColorTexture.index);

    auto tex_it = textures.find(idx);
    if (tex_it != textures.end()) {
      material.base_color_texture = &(tex_it->second);
    } else {
      auto gltf_texture = gltf_model.textures[idx];
      auto texture = process_texture(gltf_texture);
      auto res = textures.emplace(idx, move(texture));
      material.base_color_texture = &(res.first->second);
    }
  } else {
    for (size_t i = 0; i < 4; i++) {
      material.base_color[i] = pbrt.baseColorFactor[i];
    }
  }

  //  if (pbrt.metallicRoughnessTexture.index >= 0) {
  //
  //  } else {
  //  }
  //
  //  if (gltf_material.normalTexture.index >= 0) {
  //  }
  //
  //  if (gltf_material.emissiveTexture.index >= 0) {
  //  }
  //
  //  if (gltf_material.occlusionTexture.index >= 0) {
  //  }

  return material;
}

auto GLTFModelLoader::process_primitive(const tinygltf::Primitive &primitive)
    -> Geometry {
  Geometry geometry{};

  auto &accessors = gltf_model.accessors;
  auto &attributes = primitive.attributes;

  const float *positions_buffer = nullptr;
  const float *normals_buffer = nullptr;
  const float *tex_coord_buffer = nullptr;

  if (primitive.indices >= 0) {
    auto &accessor =
        gltf_model.accessors[static_cast<uint32_t>(primitive.indices)];
    auto &buffer_view = gltf_model.bufferViews[accessor.bufferView];
    auto &buffer = gltf_model.buffers[buffer_view.buffer];
    geometry.index_count = accessor.count;

    auto data = &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
    geometry.indices.reserve(geometry.index_count);

    switch (accessor.componentType) {
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
      auto *buf = reinterpret_cast<const uint32_t *>(data);
      for (size_t index = 0; index < accessor.count; index++) {
        geometry.indices.push_back(buf[index]);
      }
      break;
    }
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
      auto buf = reinterpret_cast<const uint16_t *>(data);
      for (size_t index = 0; index < accessor.count; index++) {
        geometry.indices.push_back(buf[index]);
      }
      break;
    }
    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
      auto buf = reinterpret_cast<const uint8_t *>(data);
      for (size_t index = 0; index < accessor.count; index++) {
        geometry.indices.push_back(buf[index]);
      }
      break;
    }
    default:
      throw runtime_error("unknown indices type");
    }
  }

  {
    auto position_it = primitive.attributes.find("POSITION");
    if (position_it == attributes.end()) {
      throw runtime_error("position attribute is required");
    }
    assert(position_it->second > 0 &&
           position_it->second < gltf_model.accessors.size());
    auto idx = static_cast<uint32_t>(position_it->second);
    auto &accessor = gltf_model.accessors[idx];
    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
      throw runtime_error("position buffer should be of float type");
    }

    auto &buffer_view =
        gltf_model.bufferViews[static_cast<uint32_t>(accessor.bufferView)];
    auto &buffer =
        gltf_model.buffers[static_cast<uint32_t>(buffer_view.buffer)];
    geometry.box.min = {static_cast<float>(accessor.minValues[0]),
                        static_cast<float>(accessor.minValues[1]),
                        static_cast<float>(accessor.minValues[2])};
    geometry.box.max = {static_cast<float>(accessor.maxValues[0]),
                        static_cast<float>(accessor.maxValues[1]),
                        static_cast<float>(accessor.maxValues[2])};

    geometry.vertex_count = static_cast<uint32_t>(accessor.count);
    positions_buffer = reinterpret_cast<const float *>(
        &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]));
  }

  {
    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
      auto &accessor =
          gltf_model.accessors[primitive.attributes.find("NORMAL")->second];
      auto &buffer_view =
          gltf_model.bufferViews[static_cast<uint32_t>(accessor.bufferView)];
      auto &buffer =
          gltf_model.buffers[static_cast<uint32_t>(buffer_view.buffer)];

      normals_buffer = reinterpret_cast<const float *>(
          &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]));
    }
  }

  {
    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
      auto &accessor =
          gltf_model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
      auto &buffer_view =
          gltf_model.bufferViews[static_cast<uint32_t>(accessor.bufferView)];
      auto &buffer =
          gltf_model.buffers[static_cast<uint32_t>(buffer_view.buffer)];

      tex_coord_buffer = reinterpret_cast<const float *>(
          &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]));
    }
  }

  for (size_t i = 0; i < geometry.vertex_count; i++) {
    Vertex vertex{};
    vertex.position = {positions_buffer[i * 3], positions_buffer[i * 3 + 1],
                       positions_buffer[i * 3 + 2]};
    if (normals_buffer != nullptr) {
      vertex.normal = {normals_buffer[i * 3], normals_buffer[i * 3 + 1],
                       normals_buffer[i * 3 + 2]};
    }
    if (tex_coord_buffer != nullptr) {
      vertex.uv = {tex_coord_buffer[i * 2], tex_coord_buffer[i * 2 + 1]};
    }
    geometry.buffers.emplace_back(move(vertex));
  }

  return geometry;
}

auto GLTFModelLoader::process_mesh(const tinygltf::Mesh &gltf_mesh) -> Mesh {
  auto &gltf_primitives = gltf_mesh.primitives;

  Mesh mesh{};
  for (auto &gltf_primitive : gltf_primitives) {
    if (gltf_primitive.mode != TINYGLTF_MODE_TRIANGLES) {
      continue;
    }
    auto geometry = process_primitive(gltf_primitive);
    if (geometry.box.min[0] < mesh.box.min[0]) {
      mesh.box.min = geometry.box.min;
    }
    if (geometry.box.max[0] > mesh.box.max[0]) {
      mesh.box.max = geometry.box.max;
    }

    // TODO: handle material
    if (gltf_primitive.material >= 0) {
      auto gltf_material =
          gltf_model.materials[static_cast<size_t>(gltf_primitive.material)];
      geometry.material = process_material(gltf_material);
    }

    mesh.geometries.emplace_back(move(geometry));
  }

  return mesh;
}

auto process_camera(const tinygltf::Camera &gltf_camera) -> unique_ptr<Camera> {
  auto camera = make_unique<Camera>();

  if (gltf_camera.type == "perspective") {
    auto &params = gltf_camera.perspective;
    auto fov = params.yfov;
    auto z_near = params.znear;
    auto z_far = params.zfar < 0 ? 1.0f : params.zfar;
    auto aspect = params.aspectRatio;
    camera->setProjection(fov, aspect, z_near, z_far);
    return camera;
  }

  if (gltf_camera.type == "orthographic") {
    auto &params = gltf_camera.orthographic;
    auto w = static_cast<float>(params.xmag) / 2.0f;
    auto h = static_cast<float>(params.ymag) / 2.0f;
    auto z_near = static_cast<float>(params.znear);
    auto z_far = static_cast<float>(params.zfar);
    camera->setProjection(Camera::Projection::Orthographic, -w, w, h, -h,
                          z_near, z_far);
    return camera;
  }

  throw runtime_error("unsupported camera");
}

void GLTFModelLoader::process_node(const tinygltf::Node &gltf_node,
                                   const Matrix4f *parent_transform) {
  Affine3f transform = Affine3f::Identity();

  if (!gltf_node.matrix.empty()) {
    for (size_t i = 0; i < gltf_node.matrix.size(); i++) {
      transform.data()[i] = static_cast<float>(gltf_node.matrix[i]);
    }
  } else {
    Vector3f scale{1.0f, 1.0f, 1.0f};
    for (size_t i = 0; i < gltf_node.scale.size(); i++) {
      scale[i] = gltf_node.scale[i];
    }

    Quaternionf rotation(1.0f, 0.0f, 0.0f, 0.0f);
    if (gltf_node.rotation.size() >= 4) {
      rotation.w() = gltf_node.rotation[3];
      rotation.x() = gltf_node.rotation[0];
      rotation.y() = gltf_node.rotation[1];
      rotation.z() = gltf_node.rotation[2];
    }

    Translation3f translation{0.0f, 0.0f, 0.0f};
    for (size_t i = 0; i < gltf_node.translation.size(); i++) {
      translation.vector()[i] = gltf_node.translation[i];
    }

    transform.prescale(scale);
    transform.prerotate(rotation);
    transform.pretranslate(translation.vector());
  }

  Matrix4f matrix = transform.matrix();
  if (parent_transform != nullptr) {
    matrix = (*parent_transform) * matrix;
  }

  if (gltf_node.mesh >= 0) {
    auto &gltf_mesh = gltf_model.meshes[static_cast<uint32_t>(gltf_node.mesh)];

    auto mesh = process_mesh(gltf_mesh);
    mesh.set_model_matrix(matrix);
    model.meshes.emplace_back(move(mesh));
  }

  for (auto child : gltf_node.children) {
    if (child < 0) {
      continue;
    }
    auto &node = gltf_model.nodes[static_cast<uint32_t>(child)];
    process_node(node, &matrix);
  }
}

Model &GLTFModelLoader::load() {
  tinygltf::TinyGLTF loader;

  string err, warn;

  auto ext_idx = path.find_last_of('.');
  if (ext_idx == string::npos) {
    throw runtime_error("should have extension");
  }

  auto ext = path.substr(ext_idx);
  auto res = false;
  if (ext == ".gltf") {
    res = loader.LoadASCIIFromFile(&gltf_model, &err, &warn, path);
  } else if (ext == ".glb") {
    res = loader.LoadBinaryFromFile(&gltf_model, &err, &warn, path);
  } else {
    throw runtime_error("unknown file extension");
  }

  if (!warn.empty()) {
    cout << "Load GLTF model warnnig: " << warn << endl;
  }

  if (!err.empty()) {
    throw runtime_error(err);
  }

  if (!res) {
    throw runtime_error("cannot load GLTF model");
  }

  if (gltf_model.scenes.empty()) {
    throw runtime_error("no scene");
  }

  uint32_t default_scene = 0;
  if (gltf_model.defaultScene > 0) {
    assert(gltf_model.defaultScene < gltf_model.scenes.size());
    default_scene = static_cast<uint32_t>(gltf_model.defaultScene);
  }

  auto &scene_nodes = gltf_model.scenes[default_scene].nodes;
  if (scene_nodes.empty()) {
    throw runtime_error("scene does not have any nodes");
  }

  for (auto idx : scene_nodes) {
    if (idx < 0) {
      continue;
    }
    auto &gltf_node = gltf_model.nodes[static_cast<uint32_t>(idx)];

    process_node(gltf_node, nullptr);
  }

  for (auto &mesh : model.meshes) {
    if (mesh.box.min[0] < model.box.min[0]) {
      model.box.min = mesh.box.min;
    }
    if (mesh.box.max[0] > model.box.max[0]) {
      model.box.max = mesh.box.max;
    }
  }

  return model;
}
auto GLTFModelLoader::get_extends() const -> BoundingBox {
  BoundingBox box{};
  for (auto &mesh : model.meshes) {
    for (auto &geometry : mesh.geometries) {
      Vector4f min =
          mesh.model_matrix * Vector4f(geometry.box.min[0], geometry.box.min[1],
                                       geometry.box.min[2], 1.0f);
      min /= min[3];
      Vector4f max =
          mesh.model_matrix * Vector4f(geometry.box.max[0], geometry.box.max[1],
                                       geometry.box.max[2], 1.0f);
      max /= max[3];
      Vector4f center = (min + max) / 2.0f;
      Vector4f distance = max - center;
      distance[3] = 0.0f;
      float radius = distance.norm();
      min = center - Vector4f(radius, radius, radius, radius);
      max = center + Vector4f(radius, radius, radius, radius);
      for (size_t i = 0; i < 3; i++) {
        box.min[i] = std::min(box.min[i], min[i]);
        box.max[i] = std::max(box.max[i], max[i]);
      }
    }
  }
  return box;
}

} // namespace RB
