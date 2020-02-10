#include <RenderBoy/Controls/Trackball.hpp>
// clang-format off
#include <glad/glad.h>
// clang-format on
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <RenderBoy/ModelLoader.hpp>
#include <RenderBoy/Renderer.hpp>

using namespace std;
using namespace Eigen;
using namespace RB;

constexpr const char *vertex_shader_text =
    "#version 330 core\n"
    "layout (location = 0) in vec2 a_pos;\n"
    "layout (location = 1) in vec2 a_uv;\n"
    "out vec2 v_uv;"
    "void main() {\n"
    "    v_uv = a_uv;\n"
    "    gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}";

constexpr const char *fragment_shader_text =
    "#version 330 core\n"
    "in vec2 v_uv;\n"
    "uniform sampler2D u_texture;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = texture(u_texture, v_uv);\n"
    "}";

static void error_callback(int error, const char *description);
static void key_callback(GLFWwindow *window, int key, int scan_code, int action,
                         int mods);
static void cursor_pos_callback(GLFWwindow *window, double, double);
static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int mods);
static void scroll_callback(GLFWwindow *, double, double);
static GLuint create_shader(GLenum type, const char *shader_text);
static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader);
static GLuint create_texture(int width, int height, const unsigned char *data);

TrackballControl control(800, 600);

int main(int argc, const char **argv) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " /path/to/config/file" << endl;
    return 1;
  }

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwSetErrorCallback(error_callback);

  uint32_t width = 800;
  uint32_t height = 600;
  auto *window =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                       "RenderBoy", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);

  if (!gladLoadGL()) {
    cerr << "load GL failed." << endl;
    return 0;
  }

  auto vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_text);
  auto fragment_shader =
      create_shader(GL_FRAGMENT_SHADER, fragment_shader_text);
  auto program = create_program(vertex_shader, fragment_shader);

  // clang-format off
  vector<float> positions{
      -1.0f, 1.0f,
      1.0f, -1.0f,
      -1.0f, -1.0f,
      -1.0f, 1.0f,
      1.0f, 1.0f,
      1.0f,  -1.0f,
  };
  // clang-format on

  GLuint position_buffer;
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
               reinterpret_cast<void *>(positions.data()), GL_STATIC_DRAW);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  auto pos_location = glGetAttribLocation(program, "a_pos");
  glEnableVertexAttribArray(pos_location);
  glVertexAttribPointer(pos_location, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  // clang-format off
  vector<float> tex_coords{
      0.0f, 1.0f,
      1.0f, 0.0f,
      0.0f, 0.0f,
      0.0f, 1.0f,
      1.0f, 1.0f,
      1.0f, 0.0f
  };
  // clang-format on

  GLuint tex_coords_buffer;
  glGenBuffers(1, &tex_coords_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, tex_coords_buffer);
  glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(float),
               reinterpret_cast<void *>(tex_coords.data()), GL_STATIC_DRAW);
  auto tex_coord_location = glGetAttribLocation(program, "a_uv");
  glEnableVertexAttribArray(tex_coord_location);
  glVertexAttribPointer(tex_coord_location, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glUseProgram(program);

  auto texture = create_texture(width, height, nullptr);
  auto textureLocation = glGetUniformLocation(program, "u_texture");
  glUniform1i(textureLocation, 0);

  ModelLoader loader((string(argv[1])));
  auto model = loader.load();

  Renderer renderer(Renderer::Type::CPURasterizer);
  renderer.set_frame(width, height);

  Camera camera{};
  camera.setProjection(45.0f,
                       static_cast<float>(width) / static_cast<float>(height),
                       0.01f, 1000.0f);
  Vector3f center = (model.box.max + model.box.min) / 2.0f;
  control.position = {0.0f, 0.0f, 10.0f};
  control.target = {0.0f, 0.0f, 0.0f};
  //  control.position[2] = model.box.max[2] + 5.0f;
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    auto start = chrono::steady_clock::now();

    camera.lookAt(control.position, control.target, control.up);
    auto &frame = renderer.render(model, camera);
    auto &colors = frame.getColors();

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT,
                 colors.data());

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << "\r" << to_string(delta.count()) + " ms";

    glDrawArrays(GL_TRIANGLES, 0, 6);

    control.update();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void error_callback(int, const char *description) {
  cout << "glfw error: " << description << endl;
}

void key_callback(GLFWwindow *window, int key, int scan_code, int action,
                  int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
}

void cursor_pos_callback(GLFWwindow *window, double x, double y) {
  control.onMouseMove(static_cast<int>(x), static_cast<int>(y));
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
  if (y_offset == 0.0) {
    return;
  }
  control.onMouseWheelScroll(static_cast<float>(y_offset));
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  double x, y;
  glfwGetCursorPos(window, &x, &y);

  if (action == GLFW_PRESS) {
    control.onMouseDown(GLFW_MOUSE_BUTTON_LEFT == button, static_cast<int>(x),
                        static_cast<int>(y));
  } else if (action == GLFW_RELEASE) {
    control.onMouseUp(GLFW_MOUSE_BUTTON_LEFT == button, static_cast<int>(x),
                      static_cast<int>(y));
  }
}

GLuint create_shader(GLenum type, const char *shader_text) {
  auto shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_text, nullptr);
  glCompileShader(shader);

  int success;
  char log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, log);
    throw runtime_error(string("compile shader error: ") + log);
  };

  return shader;
}

GLuint create_program(GLuint vertex_shader, GLuint fragment_shader) {
  auto program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  int success;
  char log[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, nullptr, log);
    throw runtime_error(string("link program error: ") + log);
  }

  return program;
}

GLuint create_texture(int width, int height, const unsigned char *data) {
  GLuint texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  return texture;
}
