#pragma once
#include <cfloat>
#include <cstdint>
#include <iostream>
#include <vector>

class Frame {
public:
  Frame(uint32_t width, uint32_t height)
      : width(width), height(height), size(width * height),
        zBuffer(width * height, FLT_MIN), colors(width * height * 4, 0.0f) {}
  std::vector<float> colors;
  uint32_t size;

private:
  uint32_t width;
  uint32_t height;
  std::vector<float> zBuffer;
};
