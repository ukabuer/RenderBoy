#include "Model.hpp"
#include "Material/Material.hpp"
#include "Material/DepthMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <stb_image.h>
#include <utility>
#include <vector>

using namespace std;

vector<shared_ptr<Texture>>
loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                     string typeName,
                     vector<shared_ptr<Texture>> &texturesLoaded) {
  vector<shared_ptr<Texture>> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    auto skip = false;
    for (auto &j : texturesLoaded) {
      if (std::strcmp(j->path.data(), str.C_Str()) == 0) {
        textures.push_back(j);
        skip = true;
        break;
      }
    }
    if (!skip) {
      auto texture = make_shared<Texture>();
      int width, height, nrComponents;
      texture->data = stbi_load(str.C_Str(), &width, &height, &nrComponents, 0);
      texture->width = width;
      texture->height = height;
      texture->channels = nrComponents;
      texture->type = typeName;
      texture->path = str.C_Str();
      texturesLoaded.push_back(texture);
      textures.push_back(texturesLoaded.back());
    }
  }
  return textures;
}

unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene,
                             vector<shared_ptr<Texture>> &texturesLoaded) {
  std::vector<Eigen::Vector3f> vertices;
  std::vector<Eigen::Vector3f> normals;
  std::vector<uint32_t> indices;
  std::vector<Eigen::Vector2f> texCoords;
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y,
                          mesh->mVertices[i].z);
    if (mesh->mNormals != nullptr) {
      normals.emplace_back(mesh->mNormals[i].x, mesh->mNormals[i].y,
                           mesh->mNormals[i].z);
    }
    if (mesh->mTextureCoords[0]) {
      texCoords.emplace_back(mesh->mTextureCoords[0][i].x,
                             mesh->mTextureCoords[0][i].y);
    }
  }

  shared_ptr<Material> material = make_shared<DepthMaterial>();
  if (mesh->mMaterialIndex > 0) {
    const auto mat = scene->mMaterials[mesh->mMaterialIndex];
    auto diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE,
                                       "texture_diffuse", texturesLoaded);
    material = make_shared<PhongMaterial>(diffuseMaps);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const auto face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  return make_unique<Mesh>(make_unique<Geometry>(move(vertices), move(indices),
                                                 move(normals),
                                                 move(texCoords)),
                           move(material));
}

static void findMesh(vector<shared_ptr<Mesh>> &meshes, aiNode *node,
                     const aiScene *scene,
                     vector<shared_ptr<Texture>> &texturesLoaded) {
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    const auto mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene, texturesLoaded));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    findMesh(meshes, node->mChildren[i], scene, texturesLoaded);
  }
}

static void import(vector<shared_ptr<Mesh>> &meshes, const std::string &file,
                   vector<shared_ptr<Texture>> &texturesLoaded) {
  Assimp::Importer importer;

  const auto scene = importer.ReadFile(
      file, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    cerr << importer.GetErrorString() << endl;
    return;
  }

  findMesh(meshes, scene->mRootNode, scene, texturesLoaded);
}

Model::Model(const string &path) { import(this->meshes, path, texturesLoaded); }
