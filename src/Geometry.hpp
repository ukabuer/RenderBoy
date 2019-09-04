#pragma once

#include <Eigen/Core>
#include <memory>
#include <vector>

class Mesh;
class Renderer;

class Geometry {
  friend class Mesh;

public:
  explicit Geometry(std::vector<Eigen::Vector3f> v);

  Geometry(std::vector<Eigen::Vector3f> v, std::vector<uint32_t> indices);

  Geometry(std::vector<Eigen::Vector3f> v, std::vector<uint32_t> indices,  std::vector<Eigen::Vector3f> normals);

  inline const std::vector<Eigen::Vector3f> &getVertices() const {
    return vertices;
  }

  inline const std::vector<uint32_t> &getIndicess() const { return indices; }

  inline const std::vector<Eigen::Vector2f> &getTexCoords() const {
    return texCoords;
  }

  static std::unique_ptr<Geometry> Box(float width, float height, float depth);

private:
  std::vector<uint32_t> indices;
  std::vector<Eigen::Vector3f> vertices;
  std::vector<Eigen::Vector3f> normals;
  std::vector<Eigen::Vector2f> texCoords;
};
