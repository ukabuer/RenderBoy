#pragma once
#include <Eigen/Core>

enum class LightType : uint8_t {
  Ambient,
  Point,
  Direction,
  Spot,
  None,
};

struct Light {
  LightType type = LightType::None;
  float intensity = 1.0f;
  Eigen::Vector3f color = {1.f, 1.f, 1.f};
  Eigen::Vector3f position = {0.f, 0.f, 0.f};
  bool enabled = true;

  static auto Ambient() -> Light { return {LightType::Ambient}; }

  static auto Point() -> Light { return {LightType::Point}; }
};
