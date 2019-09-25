#pragma once
#include <Eigen/Core>
#include <array>

struct Vertex {
  Eigen::Vector3f position = {0.f, 0.f, 0.f};
  Eigen::Vector3f normals = {0.f, 0.f, 0.f};
  std::array<float, 2> uv = {0.f, 0.f};
  std::array<int, 2> screen = {0, 0};
  float depth = 0.0f;
  Eigen::Vector3f T = Eigen::Vector3f::Zero();
  Eigen::Vector3f B = Eigen::Vector3f::Zero();
};

struct Point {
  int x = 0;
  int y = 0;
  float z = 0.0f;
  Eigen::Vector2f uv = {0.0f, 0.0f};
  Eigen::Vector3f normals = {0.0f, 0.0f, 0.0f};
  Eigen::Vector3f position = {0.0f, 0.0f, 0.0f};
};
