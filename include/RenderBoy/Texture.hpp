#pragma once
#include <Eigen/Core>
#include <algorithm>
#include <string>

struct Texture {
  uint32_t width = 0;
  uint32_t height = 0;
  uint8_t channels = 0;
  unsigned char *data = nullptr;
  Eigen::Vector4f sample(float u, float v) {
    u = u - std::floor(u);
    v = v - std::floor(v);
    u = std::max(std::min(u, 1.0f), 0.0f);
    v = std::max(std::min(v, 1.0f), 0.0f);

    auto offset =
        static_cast<size_t>(u * static_cast<float>(width - 1)) +
        static_cast<size_t>(v * static_cast<float>(height - 1)) * width;
    offset = channels == 4 ? offset << 2u : offset * 3;

    Eigen::Vector4f res = {static_cast<float>(data[offset]) / 255.0f,
                           static_cast<float>(data[offset + 1]) / 255.0f,
                           static_cast<float>(data[offset + 2]) / 255.0f, 1.0f};
    if (channels == 4) {
      res[3] = static_cast<float>(data[offset + 3]) / 255.0f;
    }

    return res;
  }
};
