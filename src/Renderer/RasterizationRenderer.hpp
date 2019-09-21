#pragma once
#include "Renderer/Renderer.hpp"

class RasterizationRenderer final : public Renderer {
public:
  void render(const Scene &scene, Camera &camera) override;
};
