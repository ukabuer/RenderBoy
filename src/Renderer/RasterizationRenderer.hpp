#pragma once

#include "Renderer/Renderer.hpp"
#include <array>

struct Point {
  int x = 0;
  int y = 0;
  float z = 0.0f;
  float u = 0.0f;
  float v = 0.0f;
  std::array<float, 3> normal = {0.0f, 0.0f, 0.0f };
};

class RasterizationRenderer final : public Renderer {
public:
  Frame render(const Scene &scene, const Camera &camera) override;
};
