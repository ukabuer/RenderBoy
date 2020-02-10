#pragma once
#include <RenderBoy/Texture.hpp>
#include <array>
#include <memory>

namespace RB {

struct Material {
  std::array<float, 4> base_color = {0.0f, 0.0f, 0.0f, 0.0f};
  float AlphaCutoff = 1.0f;
  float metallic = 1.0f;
  float roughness = 1.0f;
  std::array<float, 4> emissive = {1.0f, 1.0f, 1.0f, 1.0f};

  Texture *base_color_texture = nullptr;
};

} // namespace RB