#pragma once

#include <Eigen/Core>
#include <algorithm>
#include <string>

struct Texture {
  float *data = nullptr;
  size_t width = 0;
  size_t height = 0;
  size_t channels = 3;
  std::string type;
  std::string path;

  Eigen::Vector3f getColor(float u, float v) const {
    u = std::max(std::min(u, 1.f), 0.f);
    v = std::max(std::min(v, 1.f), 0.f);

    auto idx = static_cast<size_t>(u * (width - 1)) +
               static_cast<size_t>(v * (height - 1)) * width;
    idx = channels == 4 ? idx << 2 : idx * 3;

    return Eigen::Vector3f{data[idx], data[idx + 1], data[idx + 2]};
  }

  ~Texture() {
    if (data == nullptr) {
      return;
    }

    delete data;
  }
};
