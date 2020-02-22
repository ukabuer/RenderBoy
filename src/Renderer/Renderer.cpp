#include <RenderBoy/Renderer.hpp>
#include <Renderer/CPURasterizer/RasterizationRenderer.hpp>
#include <memory>

namespace RB {

Renderer::Renderer(Renderer::Type type) {
  switch (type) {
  case Type::CPURasterizer: {
    impl = std::make_unique<RasterizationRenderer>();
    break;
  }
  }
}

const Frame &Renderer::render(const Model &model, const Camera &camera) {
  return impl->render(model, camera);
}

void Renderer::set_frame(uint32_t width, uint32_t height) {
  return impl->set_frame(width, height);
}

Renderer::~Renderer() = default;

} // namespace RB