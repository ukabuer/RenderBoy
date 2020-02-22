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
#include <RenderBoy/Context.hpp>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;
using namespace RB;

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

  ModelLoader loader((string(argv[1])));
  auto model = loader.load();
  auto extends = loader.get_extends();

  Camera camera{};
  camera.setProjection(45.0f,
                       static_cast<float>(width) / static_cast<float>(height),
                       0.01f, 1000.0f);
  control.target = (extends.min + extends.max) / 2.0f;
  control.position = control.target;
  auto radius = (extends.max - extends.min).norm();
  control.position[2] += radius;

  Context context(Context::Type::OpenGL);
  context.view_port(width, height);
  context.add(model);

  while (!glfwWindowShouldClose(window)) {
    auto start = chrono::steady_clock::now();

    camera.lookAt(control.position, control.target, control.up);

    const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
    const Matrix4f &projectionMatrix = camera.getCullingProjectionMatrix();
    Matrix4f view_matrix = projectionMatrix * viewMatrix;
    context.set_view(view_matrix);
    context.draw();

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << "\r" << to_string(delta.count()) + " ms";

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
