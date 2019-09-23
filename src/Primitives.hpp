#pragma once
#include <Eigen/Core>

struct Point {
  int x = 0;
  int y = 0;
  float z = 0.0f;
  Eigen::Vector2f uv = {0.0f, 0.0f};
  Eigen::Vector3f normals = {0.0f, 0.0f, 0.0f};
  Eigen::Vector3f position = {0.0f, 0.0f, 0.0f};
};
