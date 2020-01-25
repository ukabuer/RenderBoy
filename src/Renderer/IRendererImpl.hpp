#pragma once
#include <RenderBoy/Camera.hpp>
#include <RenderBoy/Frame.hpp>
#include <RenderBoy/Model.hpp>

namespace RB {

class IRendererImpl {
public:
  ~IRendererImpl() = default;

  virtual auto render(const Model &, const Camera &) -> const Frame & = 0;

  virtual void set_frame(uint32_t width, uint32_t height) = 0;

protected:
  Eigen::Vector4f background = {0.0f, 0.0f, 0.0f, 1.0f};
  Frame frame;
};

} // namespace RB