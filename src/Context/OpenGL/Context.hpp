#pragma once
#include "Context/IContextImp.hpp"
#include <array>
#include <glad/glad.h>
#include <vector>

namespace RB {

class OpenGLContext : public IContextImpl {
public:
  OpenGLContext();

  void add(const Model &model) override;
  void draw() override;
  void set_view(const Eigen::Matrix4f &view_matrix) override;
  void view_port(uint32_t width, uint32_t height) override;
  auto get_colors() -> const std::vector<float> & override;

private:
  std::vector<GLuint> vaos;
  std::vector<uint32_t> counts;
  std::vector<Eigen::Matrix4f> model_matrixs;
  std::vector<GLuint> textures;
  std::vector<bool> use_textures;
  std::vector<std::array<float, 4>> base_colors;
  GLuint program;
  GLint model_matrix_location;
  GLint view_matrix_location;
  GLint texture_location;
  GLint use_texture_location;
  GLint base_color_location;
};

} // namespace RB