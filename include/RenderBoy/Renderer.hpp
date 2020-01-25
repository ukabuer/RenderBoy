#pragma once
#include <RenderBoy/Camera.hpp>
#include <RenderBoy/Frame.hpp>
#include <RenderBoy/Model.hpp>

namespace RB {

class IRendererImpl;

class Renderer {
public:
  enum class Type : uint8_t { CPURasterizer };
  Renderer() = delete;
  explicit Renderer(Type type);
  Renderer(const Renderer &) = delete;
  Renderer(Renderer &&) = delete;
  auto operator=(const Renderer &) = delete;
  auto operator=(Renderer &&) = delete;
  ~Renderer();

  auto render(const Model &model, const Camera &camera) -> const Frame &;

  void set_frame(uint32_t width, uint32_t height);

private:
  std::unique_ptr<IRendererImpl> impl;
};

} // namespace RB
