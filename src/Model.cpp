#include "Model.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <iostream>
#include <utility>
#include <vector>

using namespace std;

unique_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene) {
  std::vector<Eigen::Vector3f> vertices;
  std::vector<uint32_t> indices;
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y,
                          mesh->mVertices[i].z);
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }

  return make_unique<Mesh>(
      make_unique<Geometry>(move(vertices), move(indices)));
}

static void findMesh(vector<shared_ptr<Mesh>> &meshes, aiNode *node,
                     const aiScene *scene) {
  for (size_t i = 0; i < node->mNumMeshes; i++) {
    auto mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    findMesh(meshes, node->mChildren[i], scene);
  }
}

static void import(vector<shared_ptr<Mesh>> &meshes, const std::string &file) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      file, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (!scene) {
    cerr << (importer.GetErrorString()) << endl;
    return;
  }

  findMesh(meshes, scene->mRootNode, scene);
}

Model::Model(const string &path) { import(this->meshes, path); }
