#pragma once

#include <Eigen/Core>
#include <vector>

struct Geometry {
  std::vector<uint32_t> indices;
  std::vector<Eigen::Vector3f> vertices;
  std::vector<Eigen::Vector3f> normals;
  std::vector<Eigen::Vector2f> texCoords;

  static std::unique_ptr<Geometry> Box(float width, float height, float depth);
};
