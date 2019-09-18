#include "Model.hpp"
#include "Material/DepthMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include "Color.hpp"
#include <iostream>
#include <stb_image.h>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace Eigen;

static shared_ptr<Texture>
loadTexture(const string &path, const string &baseDir,
            vector<shared_ptr<Texture>> &textures) {
  int width, height, channels;
  const auto filepath = path + "/" + baseDir;

  for (auto &texture : textures) {
    if (texture->path == filepath) {
      return texture;
    }
  }

  const auto data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

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

static unique_ptr<Material> loadMaterial(const aiMaterial &ai_material,
                                         vector<shared_ptr<Texture>> &textures,
                                         const string &baseDir) {
  auto material = make_unique<PhongMaterial>();

  aiColor3D diffuse;
  ai_material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
  material->diffuseColor = Color::from(diffuse.r, diffuse.g, diffuse.b);

  aiColor3D specular;
  ai_material.Get(AI_MATKEY_COLOR_SPECULAR, specular);
  material->specularColor = Color::from(specular.r, specular.g, specular.b);

  aiColor3D ambient;
  ai_material.Get(AI_MATKEY_COLOR_AMBIENT, ambient);
  material->specularColor = Color::from(ambient.r, ambient.g, ambient.b);

  aiString path;

  const auto hasDiffuseMap =
      ai_material.GetTextureCount(aiTextureType_DIFFUSE) > 0;
  if (hasDiffuseMap) {
    ai_material.GetTexture(aiTextureType_DIFFUSE, 0, &path);
    shared_ptr<Texture> diffuseMap = loadTexture(path.C_Str(), baseDir, textures);
  }

  const auto hasSpecularMap =
      ai_material.GetTextureCount(aiTextureType_SPECULAR) > 0;
  if (hasSpecularMap) {
    ai_material.GetTexture(aiTextureType_SPECULAR, 0, &path);
    shared_ptr<Texture> specularMap =
        loadTexture(path.C_Str(), baseDir, textures);
  }

  const auto hasAmbientMap =
      ai_material.GetTextureCount(aiTextureType_AMBIENT) > 0;
  if (hasAmbientMap) {
    ai_material.GetTexture(aiTextureType_AMBIENT, 0, &path);
    shared_ptr<Texture> ambientMap =
        loadTexture(path.C_Str(), baseDir, textures);
  }

  return material;
}

static unique_ptr<Mesh>
loadMesh(const aiMesh &ai_mesh, const vector<shared_ptr<Material>> &materials) {
  auto vertices = vector<Eigen::Vector3f>();
  auto normals = vector<Eigen::Vector3f>();
  auto texCoords = vector<Eigen::Vector2f>();
  auto indices = vector<uint32_t>();

  if (ai_mesh.HasFaces()) {
    for (auto i = 0u; i < ai_mesh.mNumFaces; i++) {
      auto &face = ai_mesh.mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; j++) {
        indices.push_back(face.mIndices[j]);
      }
    }
  }

  if (ai_mesh.HasPositions()) {
    for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
      vertices.emplace_back(ai_mesh.mVertices[i].x, ai_mesh.mVertices[i].y,
                            ai_mesh.mVertices[i].z);
    }
  }

  if (ai_mesh.HasNormals()) {
    for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
      normals.emplace_back(ai_mesh.mNormals[i].x, ai_mesh.mNormals[i].y,
                           ai_mesh.mNormals[i].z);
    }
  }

  if (ai_mesh.HasTextureCoords(0)) {
    for (auto i = 0u; i < ai_mesh.mNumVertices; i++) {
      texCoords.emplace_back(ai_mesh.mTextureCoords[0][i].x,
                             ai_mesh.mTextureCoords[0][i].y);
    }
  }

  auto material = materials.empty() ? make_shared<DepthMaterial>()
                                    : materials[ai_mesh.mMaterialIndex];
  auto geometry = make_unique<Geometry>(move(vertices), move(indices),
                                        move(normals), move(texCoords));
  return make_unique<Mesh>(move(geometry), material);
}

Model::Model(const string &filepath) {
  const auto lastSlash = filepath.find_last_of("/\\");
  if (lastSlash == std::string::npos) {
    this->baseDir = "";
  } else {
    this->baseDir = filepath.substr(0, lastSlash);
  }

  Assimp::Importer importer;

  const auto scene = importer.ReadFile(
      filepath, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    cerr << importer.GetErrorString() << endl;
    return;
  }

  if (scene->HasMaterials()) {
    for (auto i = 0u; i < scene->mNumMaterials; i++) {
      shared_ptr<Material> material =
          loadMaterial(*scene->mMaterials[i], textures, baseDir);
      materials.emplace_back(material);
    }
  }

  if (scene->HasMeshes()) {
    for (auto i = 0u; i < scene->mNumMeshes; i++) {
      shared_ptr<Mesh> mesh = loadMesh(*scene->mMeshes[i], materials);
      meshes.emplace_back(mesh);
    }
  }
}
