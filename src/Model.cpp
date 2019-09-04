#include "Model.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       string typeName,
                                       vector<Texture> &texturesLoaded) {
  vector<Texture *> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
      if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(&texturesLoaded[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      Texture texture;
      int width, height, nrComponents;
      texture.data = stbi_load(str.C_Str(), &width, &height, &nrComponents, 0);
      texture.width = width;
      texture.height = height;
      texture.type = typeName;
      texture.path = str.C_Str();
      texturesLoaded.push_back(texture);
      textures.push_back(&(texturesLoaded.back()));
    }
  }
  return textures;
}

unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene,
                             vector<Texture> &texturesLoaded) {
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

  vector<Texture *> diffuseMaps;
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                       "texture_diffuse", texturesLoaded);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  return make_unique<Mesh>(make_unique<Geometry>(move(vertices), move(indices), move(normals)),
                           move(diffuseMaps));
}

static void findMesh(vector<shared_ptr<Mesh>> &meshes, aiNode *node,
                     const aiScene *scene, vector<Texture> &texturesLoaded) {
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    auto mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene, texturesLoaded));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    findMesh(meshes, node->mChildren[i], scene, texturesLoaded);
  }
}

static void import(vector<shared_ptr<Mesh>> &meshes, const std::string &file,
                   vector<Texture> &texturesLoaded) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      file, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    cerr << (importer.GetErrorString()) << endl;
    return;
  }

  findMesh(meshes, scene->mRootNode, scene, texturesLoaded);
}

Model::Model(const string &path) { import(this->meshes, path, texturesLoaded); }
