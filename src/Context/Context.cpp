#include "Context/SoftwareRasterizer/Context.hpp"
#include "Context/OpenGL/Context.hpp"
#include <RenderBoy/Context.hpp>
#include <memory>

using namespace std;

namespace RB {

Context::Context(Type type) {
  switch (type) {
  case Type::SoftwareRasterizer: {
    impl = make_unique<SoftwareRasterizerContext>();
    break;
  }
  case Type::OpenGL: {
    impl = make_unique<OpenGLContext>();
  }
  }
}

Context::~Context() = default;

void Context::add(const Model &model) { impl->add(model); }

void Context::draw() { impl->draw(); }

void Context::set_view(const Eigen::Matrix4f &view_matrix) {
  impl->set_view(view_matrix);
}

void Context::view_port(uint32_t width, uint32_t height) {
  impl->view_port(width, height);
}

auto Context::get_colors() -> const std::vector<float> & {
  return impl->get_colors();
}

} // namespace RB