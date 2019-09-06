#pragma once
#include <cfloat>
#include <cstdint>
#include <vector>

class Frame {
public:
  std::vector<float> colors;
  std::vector<float> zBuffer;
  uint32_t size;

  Frame(uint32_t width, uint32_t height)
    : colors(width * height * 4, 0.0f), zBuffer(width * height, -FLT_MAX),
      size(width * height),
      width(width), height(height) {
  }

private:
  uint32_t width;
  uint32_t height;
};
