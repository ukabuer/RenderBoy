#pragma once

#include "Renderer/Renderer.hpp"

class RasterizationRenderer final : public Renderer {
public:
  Frame render(const Scene &scene, const Camera &camera) override;
};
