#include "GLTFViewerSoftware.hpp"
#include "GLUtils.hpp"
#include "glad/glad.h"
#include <Eigen/Geometry>
#include <RenderBoy/ModelLoader.hpp>

using namespace std;
using namespace Eigen;

namespace RB {

Application *create_application() { return new GLTFViewerSoftware(); }

void GLTFViewerSoftware::process_cmd(int argc, const char **argv) {
  if (argc < 2) {
    throw runtime_error(string("Usage: ") + argv[0] + " /path/to/config/file");
  }

  path = argv[1];
}

void GLTFViewerSoftware::prepare() {
  auto window_size = get_size();
  auto width = window_size[0];
  auto height = window_size[1];

  loader = make_unique<ModelLoader>(path);
  model = loader->load();
  auto extends = loader->get_extends();

  context = make_unique<Context>(Context::Type::SoftwareRasterizer);
  context->view_port(width, height);
  context->add(model);

  camera.setProjection(45.0f,
                       static_cast<float>(width) / static_cast<float>(height),
                       0.01f, 1000.0f);
  control.target = (extends.min + extends.max) / 2.0f;
  control.position = control.target;
  auto radius = (extends.max - extends.min).norm();
  control.position[2] += radius;

  glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  GLuint framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  texture = create_texture(width, height, nullptr);
}

void GLTFViewerSoftware::render() {
  auto window_size = get_size();
  auto width = window_size[0];
  auto height = window_size[1];

  glClear(GL_COLOR_BUFFER_BIT);

  camera.lookAt(control.position, control.target, control.up);
  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f &projectionMatrix = camera.getCullingProjectionMatrix();
  Matrix4f view_matrix = projectionMatrix * viewMatrix;
  context->set_view(view_matrix);
  context->draw();
  auto &colors = context->get_colors();

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT,
               colors.data());
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture, 0);
  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  control.update();
}

} // namespace RB