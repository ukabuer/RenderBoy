#include "Context/OpenGL/Context.hpp"
#include "Context/OpenGL/utils.hpp"
#include <iostream>
#include <stdexcept>

using namespace std;

namespace RB {

const char *vertex_shader_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec3 a_normal;\n"
    "layout (location = 2) in vec2 a_uv;\n"
    "\n"
    "uniform mat4 model_matrix;\n"
    "uniform mat4 view_matrix;\n"
    "\n"
    "out vec2 v_uv;\n"
    "\n"
    "void main() {\n"
    "    v_uv = a_uv;\n"
    "    gl_Position = view_matrix * model_matrix * vec4(a_pos, 1.0);\n"
    "}";

const char *fragment_shader_source =
    "#version 330 core\n"
    "in vec2 v_uv;\n"
    "uniform int use_texture;\n"
    "uniform sampler2D u_texture;\n"
    "uniform vec4 base_color;\n"
    "out vec4 color;\n"
    "\n"
    "void main() {\n"
    "    if (use_texture == 1) {\n"
    "        color = texture(u_texture, v_uv);\n"
    "    } else {\n"
    "        color = base_color;\n"
    "    }\n"
    "}";

OpenGLContext::OpenGLContext() {
  if (!gladLoadGL()) {
    throw runtime_error("Failed to load GL");
  }

  auto vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_source);
  auto fragment_shader =
      create_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
  program = create_program(vertex_shader, fragment_shader);
  model_matrix_location = glGetUniformLocation(program, "model_matrix");
  view_matrix_location = glGetUniformLocation(program, "view_matrix");
  texture_location = glGetUniformLocation(program, "u_texture");
  use_texture_location = glGetUniformLocation(program, "use_texture");
  base_color_location = glGetUniformLocation(program, "base_color");

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
};

void OpenGLContext::add(const Model &model) {
  auto origin_vao_num = vaos.size();
  auto geometry_num = 0;
  for (auto &mesh : model.meshes) {
    geometry_num += mesh.geometries.size();
  }

  vaos.resize(origin_vao_num + geometry_num);
  model_matrixs.resize(origin_vao_num + geometry_num);
  counts.resize(origin_vao_num + geometry_num);
  textures.resize(origin_vao_num + geometry_num);
  use_textures.resize(origin_vao_num + geometry_num);
  base_colors.resize(origin_vao_num + geometry_num);
  glGenVertexArrays(geometry_num, vaos.data() + origin_vao_num);

  auto idx = origin_vao_num;
  for (auto &mesh : model.meshes) {
    auto &model_matrix = mesh.model_matrix;
    for (auto &geometry : mesh.geometries) {
      glBindVertexArray(vaos[idx]);
      model_matrixs[idx] = model_matrix;
      if (geometry.material.base_color_texture == nullptr) {
        use_textures[idx] = false;
        base_colors[idx] = geometry.material.base_color;
      } else {
        use_textures[idx] = true;
        textures[idx] = create_texture(*geometry.material.base_color_texture);
      }
      auto &material = geometry.material;
      if (geometry.index_count != 0) {
        GLuint buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     geometry.index_count * sizeof(uint32_t),
                     geometry.indices.data(), GL_STATIC_DRAW);
        counts[idx] = geometry.index_count;
      }

      auto vertex_num = geometry.vertex_count;
      GLuint buffer;
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(Vertex),
                   geometry.buffers.data(), GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), nullptr);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float),
                            reinterpret_cast<void *>(3 * sizeof(float)));
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float),
                            reinterpret_cast<void *>(6 * sizeof(float)));

      idx += 1;
    }
  }
}

void OpenGLContext::draw() {
  glUseProgram(program);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for (size_t i = 0; i < vaos.size(); i++) {
    auto vao = vaos[i];
    auto &model_matrix = model_matrixs[i];
    if (use_textures[i]) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textures[i]);
      glUniform1i(use_texture_location, 1);
      glUniform1i(texture_location, 0);
    } else {
      glUniform1i(use_texture_location, 0);
      glUniform4fv(base_color_location, 1, base_colors[i].data());
    }
    glUniformMatrix4fv(model_matrix_location, 1, false, model_matrix.data());
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, counts[i], GL_UNSIGNED_INT, nullptr);
  }
}

void OpenGLContext::set_view(const Eigen::Matrix4f &view_matrix) {
  glUseProgram(program);
  glUniformMatrix4fv(view_matrix_location, 1, false, view_matrix.data());
}

void OpenGLContext::view_port(uint32_t width, uint32_t height) {
  glViewport(0, 0, width, height);
}

auto OpenGLContext::get_colors() -> const vector<float> & {
  // TODO
  return {};
}

} // namespace RB