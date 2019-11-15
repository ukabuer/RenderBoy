#include "Model.hpp"
#include "Material/DepthMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
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
    throw "Load texture error, file: " + filepath;
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

static auto LoadMaterial(const aiMaterial &ai_material,
                         vector<shared_ptr<Texture>> &textures,
                         const string &baseDir) -> unique_ptr<Material> {
  auto material = make_unique<PhongMaterial>();

  aiColor3D diffuse;
  ai_material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
  material->setDiffuseColor(Vector3f{diffuse.r, diffuse.g, diffuse.b});

  aiColor3D specular;
  ai_material.Get(AI_MATKEY_COLOR_SPECULAR, specular);
  material->setSpecularColor(Vector3f{specular.r, specular.g, specular.b});

  aiColor3D ambient;
  ai_material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
  material->setAmbientColor(Vector3f{ambient.r, ambient.g, ambient.b});

  aiString path;

  const auto hasDiffuseMap =
      ai_material.GetTextureCount(aiTextureType_DIFFUSE) > 0;
  if (hasDiffuseMap) {
    ai_material.GetTexture(aiTextureType_DIFFUSE, 0, &path);
    material->setDiffuseMap(LoadTexture(path.C_Str(), baseDir, textures));
  }

  const auto hasSpecularMap =
      ai_material.GetTextureCount(aiTextureType_SPECULAR) > 0;
  if (hasSpecularMap) {
    ai_material.GetTexture(aiTextureType_SPECULAR, 0, &path);
    material->setSpecularMap(LoadTexture(path.C_Str(), baseDir, textures));
  }

  const auto hasAmbientMap =
      ai_material.GetTextureCount(aiTextureType_AMBIENT) > 0;
  if (hasAmbientMap) {
    ai_material.GetTexture(aiTextureType_AMBIENT, 0, &path);
    material->setAmbientMap(LoadTexture(path.C_Str(), baseDir, textures));
  }

  const auto hasNormalMap =
      ai_material.GetTextureCount(aiTextureType_NORMALS) > 0;
  if (hasNormalMap) {
    ai_material.GetTexture(aiTextureType_NORMALS, 0, &path);
    material->setNormalMap(LoadTexture(path.C_Str(), baseDir, textures));
  }

  auto shininess = 0.f;
  ai_material.Get(AI_MATKEY_SHININESS, shininess);
  if (shininess == 0.f) {
    material->setShininess(30.f);
  }

  return material;
}

static auto LoadMesh(const aiMesh &ai_mesh,
                     const vector<shared_ptr<Material>> &materials) -> Mesh {
  auto indices = vector<uint32_t>();
  auto vertices = vector<Vertex>(ai_mesh.mNumVertices);

  for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
    vertices[i].position = {ai_mesh.mVertices[i].x, ai_mesh.mVertices[i].y,
                            ai_mesh.mVertices[i].z};
  }

  if (ai_mesh.HasNormals()) {
    for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
      vertices[i].normals = {ai_mesh.mNormals[i].x, ai_mesh.mNormals[i].y,
                             ai_mesh.mNormals[i].z};
    }
  }

  if (ai_mesh.HasTangentsAndBitangents()) {
    for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
      vertices[i].T = {ai_mesh.mTangents[i].x, ai_mesh.mTangents[i].y,
                       ai_mesh.mTangents[i].z};

      vertices[i].B = {ai_mesh.mBitangents[i].x, ai_mesh.mBitangents[i].y,
                       ai_mesh.mBitangents[i].z};
    }
  }

  if (ai_mesh.HasTextureCoords(0)) {
    for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
      vertices[i].uv = {ai_mesh.mTextureCoords[0][i].x,
                        ai_mesh.mTextureCoords[0][i].y};
    }
  }

  if (ai_mesh.HasFaces()) {
    for (auto i = 0u; i < ai_mesh.mNumFaces; i++) {
      auto &face = ai_mesh.mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }
  } else {
    for (auto i = 0ul; i < ai_mesh.mNumVertices; i++) {
      indices.emplace_back(i);
      indices.emplace_back(i + 1);
      indices.emplace_back(i + 2);
    }
  }

  auto material = materials.empty() || !ai_mesh.HasNormals()
                      ? make_shared<DepthMaterial>()
                      : materials[ai_mesh.mMaterialIndex];
  auto geometry = make_unique<Geometry>();
  geometry->vertices = move(vertices);
  geometry->indices = move(indices);

  return {move(geometry), material};
}

auto Model::Load(const string &filepath) -> Model {
  Model model;
  const auto lastSlash = filepath.find_last_of("/\\");
  if (lastSlash == std::string::npos) {
    model.baseDir = "";
  } else {
    model.baseDir = filepath.substr(0, lastSlash);
  }

  Assimp::Importer importer;

  const auto scene = importer.ReadFile(
      filepath, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (scene == nullptr) {
    cerr << importer.GetErrorString() << endl;
    return model;
  }

  if (scene->HasMaterials()) {
    for (auto i = 0u; i < scene->mNumMaterials; i++) {
      shared_ptr<Material> material =
          LoadMaterial(*scene->mMaterials[i], model.textures, model.baseDir);
      model.materials.emplace_back(material);
    }
  }

  if (scene->HasMeshes()) {
    for (auto i = 0u; i < scene->mNumMeshes; i++) {
      auto mesh = LoadMesh(*scene->mMeshes[i], model.materials);
      model.meshes.emplace_back(mesh);
    }
  }

  return model;
}
