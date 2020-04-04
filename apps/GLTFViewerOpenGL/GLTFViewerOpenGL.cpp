#include "GLTFViewerOpenGL.hpp"
#include <Eigen/Geometry>
#include <RenderBoy/Context.hpp>
#include <RenderBoy/Controls/Trackball.hpp>
#include <RenderBoy/ModelLoader.hpp>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace Eigen;

namespace RB {

Application *create_application() { return new GLTFViewerOpenGL(); }

void GLTFViewerOpenGL::process_cmd(int argc, const char **argv) {
  if (argc < 2) {
    throw runtime_error(string("Usage: ") + argv[0] + " /path/to/config/file");
  }

  path = argv[1];
}

void GLTFViewerOpenGL::prepare() {

  ModelLoader loader(path);
  auto model = loader.load();
  auto extends = loader.get_extends();

  auto window_size = get_size();
  auto width = window_size[0];
  auto height = window_size[1];

  camera.setProjection(45.0f,
                       static_cast<float>(width) / static_cast<float>(height),
                       0.01f, 1000.0f);
  control.target = (extends.min + extends.max) / 2.0f;
  control.position = control.target;
  auto radius = (extends.max - extends.min).norm();
  control.position[2] += radius;

  context = make_unique<Context>(Context::Type::OpenGL);
  context->view_port(width, height);
  context->add(model);
}

void GLTFViewerOpenGL::render() {
  camera.lookAt(control.position, control.target, control.up);

  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f &projectionMatrix = camera.getCullingProjectionMatrix();
  Matrix4f view_matrix = projectionMatrix * viewMatrix;
  context->set_view(view_matrix);
  context->draw();
  control.update();
}

} // namespace RB