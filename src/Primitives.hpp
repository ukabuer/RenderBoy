#pragma once
#include <Eigen/Core>
#include <array>

struct Vertex {
  Eigen::Vector3f position = {0.f, 0.f, 0.f};
  Eigen::Vector3f normal = {0.f, 0.f, 0.f};
  std::array<float, 2> uv = {0.f, 0.f};
};
