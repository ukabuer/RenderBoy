#pragma once

#include <Eigen/Core>
#include <memory>
#include <vector>

class Mesh;

class Geometry {
  friend class Mesh;

public:
  explicit Geometry(std::vector<Eigen::Vector3f> v);

  Geometry(std::vector<Eigen::Vector3f> v, std::vector<uint32_t> indices);

  Geometry(std::vector<Eigen::Vector3f> v, std::vector<uint32_t> indices,
           std::vector<Eigen::Vector3f> normals);

  Geometry(std::vector<Eigen::Vector3f> v, std::vector<uint32_t> i,
           std::vector<Eigen::Vector3f> n,
           std::vector<Eigen::Vector2f> uv);

  const std::vector<Eigen::Vector3f> &getVertices() const { return vertices; }

  const std::vector<uint32_t> &getIndices() const { return indices; }

  const std::vector<Eigen::Vector3f> &getNormals() const { return normals; }

  const std::vector<Eigen::Vector2f> &getTexCoords() const { return texCoords; }

  static std::unique_ptr<Geometry> Box(float width, float height, float depth);

private:
  std::vector<uint32_t> indices;
  std::vector<Eigen::Vector3f> vertices;
  std::vector<Eigen::Vector3f> normals;
  std::vector<Eigen::Vector2f> texCoords;
};
