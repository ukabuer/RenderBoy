#pragma once
#include <Eigen/Core>
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
        size(width * height), width(width), height(height) {}

  void setColor(size_t idx, const Eigen::Vector3f &color) {
    idx = idx << 2;
    colors[idx] = color[0];
    colors[idx + 1] = color[1];
    colors[idx + 2] = color[2];
    colors[idx + 3] = 1.0f;
  }

  void setColor(uint32_t x, uint32_t y, const Eigen::Vector3f &color) {
    const auto idx = static_cast<size_t>(x + y * width);
    this->setColor(idx, color);
  }

  uint32_t getWidth() const { return width; }

  uint32_t getHeight() const { return height; }

private:
  uint32_t width;
  uint32_t height;
};
