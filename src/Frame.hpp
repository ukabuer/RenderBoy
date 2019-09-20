#pragma once
#include <Eigen/Core>
#include <cfloat>
#include <cstdint>
#include <vector>

struct Frame {
  uint32_t size = 0;
  std::vector<float> colors = {};
  std::vector<float> z = {};
};
