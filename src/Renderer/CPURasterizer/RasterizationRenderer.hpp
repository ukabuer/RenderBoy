#pragma once
#include "Eigen/Core"
#include "Rasterizer.hpp"
#include "Renderer/IRendererImpl.hpp"
#include <functional>
#include <vector>

namespace RB {

class RasterizationRenderer final : public IRendererImpl {
public:
  auto render(const Model &model, const Camera &camera)
      -> const Frame & override;

  void set_frame(uint32_t width, uint32_t height) {
    frame.resize(width, height);
  }
};

} // namespace RB
