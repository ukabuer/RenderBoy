#pragma once

#include <array>

struct Point {
  int x = 0;
  int y = 0;
  float z = 0.0f;
  float u = 0.0f;
  float v = 0.0f;
  std::array<float, 3> normals = {0.0f, 0.0f, 0.0f};
  std::array<float, 3> position = {0.0f, 0.0f, 0.0f};
};
