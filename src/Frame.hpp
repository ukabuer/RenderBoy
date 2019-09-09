#pragma once
#include <cfloat>
#include <cstdint>
#include <vector>

class Frame {
public:
  std::vector<unsigned char> colors;
  std::vector<float> zBuffer;
  uint32_t size;

  Frame(uint32_t width, uint32_t height)
    : colors(width * height * 4, 0), zBuffer(width * height, -FLT_MAX),
      size(width * height),
      width(width), height(height) {
  }

  void setColor(size_t idx, unsigned char r, unsigned char g,
                unsigned char b, unsigned char a) {
    idx = idx << 2;
    colors[idx] = r;
    colors[idx + 1] = g;
    colors[idx + 2] = b;
    colors[idx + 3] = a;
  }

  void setColor(uint32_t x, uint32_t y, unsigned char r, unsigned char g,
                unsigned char b, unsigned char a) {
    const auto idx = static_cast<size_t>(x + y * width);
    this->setColor(idx, r, g, b, a);
  }

private:
  uint32_t width;
  uint32_t height;
};
