#include "Model.hpp"
#include "Material/DepthMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <Eigen/Geometry>
#include <cassert>
#include <exception>
#include <string>
#include <tiny_gltf.h>
#include <utility>
#include <vector>

using namespace std;
using namespace Eigen;

static auto LoadTexture(const string &path, const string &baseDir,
                        vector<shared_ptr<Texture>> &textures)
    -> shared_ptr<Texture> {
  int width;
  int height;
  int channels;
  const auto filepath = baseDir + "/" + path;

  for (auto &texture : textures) {
    if (texture->path == filepath) {
      return texture;
    }
  }

  stbi_set_flip_vertically_on_load(1);
  stbi_ldr_to_hdr_scale(1.0f);
  stbi_ldr_to_hdr_gamma(1.0f);
  const auto data = stbi_loadf(filepath.c_str(), &width, &height, &channels, 0);

  if (data == nullptr) {
    throw runtime_error("Load texture error, file: " + filepath);
  }

  auto texture = make_shared<Texture>();
  texture->data = data;
  texture->width = width;
  texture->height = height;
  texture->channels = channels;
  texture->path = filepath;

  textures.push_back(texture);

  return texture;
}

static auto process_primitive(const tinygltf::Primitive &primitive,
                              const tinygltf::Model &model) -> Geometry {
  Geometry geometry{};

  auto &accessors = model.accessors;
  auto &attributes = primitive.attributes;

  const float *positions_buffer = nullptr;
  const float *normals_buffer = nullptr;
  const float *tex_coord_buffer = nullptr;

  if (primitive.indices >= 0) {
    auto &accessor = model.accessors[static_cast<uint32_t>(primitive.indices)];
    auto &buffer_view = model.bufferViews[accessor.bufferView];
    auto &buffer = model.buffers[buffer_view.buffer];
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
           position_it->second < model.accessors.size());
    auto idx = static_cast<uint32_t>(position_it->second);
    auto &accessor = model.accessors[idx];
    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
      throw runtime_error("position buffer should be of float type");
    }

    auto &buffer_view =
        model.bufferViews[static_cast<uint32_t>(accessor.bufferView)];
    auto &buffer = model.buffers[static_cast<uint32_t>(buffer_view.buffer)];
    Vector3f min_value = {static_cast<float>(accessor.minValues[0]),
                          static_cast<float>(accessor.minValues[1]),
                          static_cast<float>(accessor.minValues[2])};
    Vector3f max_value = {static_cast<float>(accessor.maxValues[0]),
                          static_cast<float>(accessor.maxValues[1]),
                          static_cast<float>(accessor.maxValues[2])};

    geometry.vertex_count = static_cast<uint32_t>(accessor.count);
    positions_buffer = reinterpret_cast<const float *>(
        &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]));
  }

  {
    if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
      auto &accessor =
          model.accessors[primitive.attributes.find("NORMAL")->second];
      auto &buffer_view =
          model.bufferViews[static_cast<uint32_t>(accessor.bufferView)];
      auto &buffer = model.buffers[static_cast<uint32_t>(buffer_view.buffer)];

      normals_buffer = reinterpret_cast<const float *>(
          &(buffer.data[accessor.byteOffset + buffer_view.byteOffset]));
    }
  }

  {
    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
      auto &accessor =
          model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
      auto &buffer_view =
          model.bufferViews[static_cast<uint32_t>(accessor.bufferView)];
      auto &buffer = model.buffers[static_cast<uint32_t>(buffer_view.buffer)];

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

static auto process_mesh(const tinygltf::Mesh &gltf_mesh,
                         const tinygltf::Model &model) -> Mesh {
  auto &gltf_primitives = gltf_mesh.primitives;

  Mesh mesh{};
  for (auto &gltf_primitive : gltf_primitives) {
    if (gltf_primitive.mode != TINYGLTF_MODE_TRIANGLES) {
      continue;
    }
    auto geometry = process_primitive(gltf_primitive, model);
    mesh.geometries.emplace_back(move(geometry));
    // TODO: handle material
  }

  return mesh;
}

static auto process_camera(const tinygltf::Camera &gltf_camera)
    -> unique_ptr<Camera> {
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

static void process_node(const tinygltf::Node &gltf_node,
                         const tinygltf::Model &model, vector<Mesh> &meshes) {
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

    //    transform = translation * rotation * scale;
  }

  if (gltf_node.mesh >= 0) {
    auto &gltf_mesh = model.meshes[static_cast<uint32_t>(gltf_node.mesh)];

    auto mesh = process_mesh(gltf_mesh, model);
    mesh.set_model_matrix(transform.matrix());
    meshes.emplace_back(move(mesh));
  }

  for (auto child : gltf_node.children) {
    if (child < 0) {
      continue;
    }
    auto &node = model.nodes[static_cast<uint32_t>(child)];
    process_node(node, model, meshes);
  }
}

auto Model::loadGLTF(const string &filepath) -> const Model & {
  tinygltf::TinyGLTF loader;
  tinygltf::Model gltf;

  string err, warn;

  auto ext_idx = filepath.find_last_of('.');
  if (ext_idx == string::npos) {
    throw runtime_error("should have extension");
  }

  auto ext = filepath.substr(ext_idx);
  auto res = false;
  if (ext == ".gltf") {
    res = loader.LoadASCIIFromFile(&gltf, &err, &warn, filepath);
  } else if (ext == ".glb") {
    res = loader.LoadBinaryFromFile(&gltf, &err, &warn, filepath);
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

  if (gltf.scenes.empty()) {
    throw runtime_error("no scene");
  }

  uint32_t default_scene = 0;
  if (gltf.defaultScene > 0) {
    assert(gltf.defaultScene < gltf.scenes.size());
    default_scene = static_cast<uint32_t>(gltf.defaultScene);
  }

  auto &scene_nodes = gltf.scenes[default_scene].nodes;
  if (scene_nodes.empty()) {
    throw runtime_error("scene does not have any nodes");
  }

  for (auto idx : scene_nodes) {
    if (idx < 0) {
      continue;
    }
    auto &gltf_node = gltf.nodes[static_cast<uint32_t>(idx)];

    process_node(gltf_node, gltf, meshes);
  }

  return *this;
}
