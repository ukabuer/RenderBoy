#pragma once
#include "Eigen/Core"
#include "Rasterizer.inl"
#include "Renderer/Renderer.hpp"
#include <functional>
#include <vector>

class RasterizationRenderer final : public Renderer {
public:
  auto render(const Model &model, const Camera &camera) -> Frame & override;
  void setFrame(uint32_t width, uint32_t height) {
    frame.resize(width, height);
  }

private:
  Frame frame;
};
