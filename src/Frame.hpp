#pragma once
#include <Eigen/Core>
#include <cfloat>
#include <cstdint>
#include <vector>

class Frame {
public:
  Frame() = default;

  Frame(uint32_t width, uint32_t height)
      : width(width), height(height), size(width * height),
        colors(size * 4, 0.0f), z(size, -FLT_MAX) {}

  void resize(uint32_t w, uint32_t h) {
    this->width = w;
    this->height = h;
    this->size = w * h;
    this->colors.resize(size * 4, 0.0f);
    this->z.resize(size, -FLT_MAX);
  }

  auto getWidth() const -> uint32_t { return width; }

  auto getHeight() const -> uint32_t { return height; }

  auto getSize() const -> uint32_t { return size; }

  auto getZ() const -> const std::vector<float> & { return z; }

  auto getZ(size_t idx) const -> float { return z[idx]; }

  void setZ(size_t idx, float z) { this->z[idx] = z; }

  void setZ(uint32_t x, uint32_t y, float z) { this->z[x + y * width] = z; }

  auto getColor(size_t idx) const -> Eigen::Vector4f {
    idx *= 4;
    return {colors[idx], colors[idx + 1], colors[idx + 2], colors[idx + 3]};
  }

  auto getColors() const -> const std::vector<float> & { return colors; }

  void setColor(uint32_t idx, const Eigen::Vector4f &color) {
    idx = idx << 2u;
    colors[idx] = color[0];
    colors[idx + 1] = color[1];
    colors[idx + 2] = color[2];
    colors[idx + 3] = 1.0f;
  }

  void setColor(uint32_t x, uint32_t y, const Eigen::Vector4f &color) {
    this->setColor(x + y * width, color);
  }

  void clear(const Eigen::Vector4f &color = {0.f, 0.f, 0.f, 1.0f},
             float z = -FLT_MAX) {
    for (auto i = 0ul; i < size; i++) {
      this->setZ(i, -FLT_MAX);
      this->setColor(i, color);
    }
  }

private:
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t size = 0;
  std::vector<float> colors;
  std::vector<float> z;
};
