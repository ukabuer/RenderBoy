#pragma once

#include <array>
#include <string>

struct Texture {
  unsigned char *data = nullptr;
  size_t width = 0;
  size_t height = 0;
  size_t channels = 3;
  std::string type;
  std::string path;

  std::array<unsigned char, 4> getColor(float u, float v) const {
    std::array<unsigned char, 4> color = {0, 0, 0, 0 };

    auto idx = static_cast<size_t>(u * width) + static_cast<size_t>(v * height) * width;
    idx = channels == 4 ? idx << 2 : idx * 3;

    color[0] = data[idx];
    color[1] = data[idx + 1];
    color[2] = data[idx + 2];
    color[3] = channels == 4 ? data[idx + 3] : 255;

    return color;
  }
};