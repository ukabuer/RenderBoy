#pragma once
#include <Eigen/Core>

struct Light {
  enum class Type : uint8_t {
    Ambient,
    Point,
    Direction,
    Spot,
    None,
  };

  Type type = Type::None;
  float intensity = 1.0f;
  Eigen::Vector3f color = {1.f, 1.f, 1.f};
  Eigen::Vector3f position = {0.f, 0.f, 0.f};
  bool enabled = true;

  static auto Ambient() -> Light { return {Light::Type::Ambient}; }

  static auto Point() -> Light { return {Light::Type::Point}; }
};
