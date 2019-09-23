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

  Texture() = default;

  Texture(const Texture &other)
      : width(other.width), height(other.height), channels(other.channels),
        type(other.type), path(other.path) {
    if (data != other.data) {
      std::memcpy(data, other.data, width * height * channels * sizeof(float));
    }
  }

  Texture(Texture &&other)
      : data(other.data), width(other.width), height(other.height),
        channels(other.channels), type(other.type), path(other.path) {
    other.data = nullptr;
  }

  auto operator=(const Texture &other) -> Texture & {
    width = other.width;
    height = other.height;
    channels = other.channels;
    type = other.type;
    path = other.path;

    if (data != other.data) {
      std::memcpy(data, other.data, width * height * channels * sizeof(float));
    }

    return *this;
  }

  auto operator=(Texture &&other) -> Texture & {
    data = other.data;
    width = other.width;
    height = other.height;
    channels = other.channels;
    type = other.type;
    path = other.path;

    other.data = nullptr;

    return *this;
  }

  ~Texture() {
    if (data == nullptr) {
      return;
    }

    delete data;
  }

  auto sample(float u, float v) const -> Eigen::Vector3f {
    u = std::max(std::min(u, 1.f), 0.f);
    v = std::max(std::min(v, 1.f), 0.f);

    auto idx = static_cast<size_t>(u * static_cast<float>(width - 1)) +
                    static_cast<size_t>(v * static_cast<float>(height - 1)) * width;
    idx = channels == 4 ? idx << 2 : idx * 3;

    return {data[idx], data[idx + 1], data[idx + 2]};
  }
};
