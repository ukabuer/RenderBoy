#pragma once
#include <Eigen/Core>
#include <cfloat>
#include <cstdint>
#include <vector>

struct Frame {
  uint32_t width;
  uint32_t height;
  uint32_t size = 0;
  float aspect = 0.f;
  std::vector<float> colors = {};
  std::vector<float> z = {};
};
