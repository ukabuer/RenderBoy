#pragma once
#include "Eigen/Core"
#include <memory>
#include <vector>

class Mesh;
class Renderer;

class Geometry {
public:
  Geometry(std::vector<Eigen::Vector3f> v, std::vector<uint32_t> indices);
  explicit Geometry(std::vector<Eigen::Vector3f> v);
  static std::unique_ptr<Geometry> Box(float width, float height, float depth);
  friend class Mesh;
  friend class Renderer;

protected:
  std::vector<Eigen::Vector3f> vertices;
  std::vector<uint32_t> indices;
};
