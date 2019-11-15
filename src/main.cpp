#include "CameraControls/OrbitControl.hpp"
#include "Renderer/RasterizationRenderer.hpp"
#include "utils.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

constexpr const char *vertex_shader_text = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;"
    "layout (location = 1) in vec2 aTexCoord;"
    "out vec2 TexCoord;"
    "void main() {"
    "    gl_Position = vec4(aPos, 0.0, 1.0);"
    "    TexCoord = aTexCoord;"
    "}";

constexpr const char *fragment_shader_text = "#version 330 core\n"
    "out vec4 FragColor;"
    "in vec2 TexCoord;"
    "uniform sampler2D ourTexture;"
    "void main() {"
    "    FragColor = texture(ourTexture, TexCoord);"
    "}";

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scan_code, int action, int mods);
static void cursor_pos_callback(GLFWwindow* window, double, double);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void scroll_callback(GLFWwindow *, double, double);
static GLuint create_shader(GLenum type, const char *shader_text);
static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader);

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

  glfwSetErrorCallback(error_callback);

  auto config = RenderConfig::Load(argv[1]);
  auto renderer = RasterizationRenderer();
//  auto controller = OrbitController(config.camera);

  // display
  const auto width = config.camera.getWidth();
  const auto height = config.camera.getHeight();
  auto *window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "RenderBoy", nullptr, nullptr);
  if (window == nullptr) {
    glfwTerminate();
    return -1;
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_pos_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);

  auto vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_text);
  auto fragment_shader = create_shader(GL_VERTEX_SHADER, fragment_shader_text);
  auto program = create_program(vertex_shader, fragment_shader);

  vector<float> vertices {
    // positions  // texture coords
    -1.0f,  1.0f,  0.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 1.0f,
  };
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size()), vertices.data(), GL_STATIC_DRAW);

  auto pos_location = glGetAttribLocation(program, "aPos");
  glEnableVertexAttribArray(pos_location);
  glVertexAttribPointer(pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

  auto tex_coord_location = glGetAttribLocation(program, "aTexCoord");
  glEnableVertexAttribArray(tex_coord_location);
  glVertexAttribPointer(tex_coord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));

  glViewport(0, 0, static_cast<int>(width), static_cast<int>(height));
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glUseProgram(program);
  glBindVertexArray(0);

  vector<unsigned char> pixels(width * height * 4);
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    auto start = chrono::steady_clock::now();

    renderer.render(config.scene, config.camera);
    auto &colors = config.camera.getFrame().getColors();
    for (decltype(colors.size()) i = 0; i < colors.size(); i++) {
      pixels[i] = static_cast<unsigned char>(colors[i] * 255.0f);
    }

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << "\r" << to_string(delta.count()) + " ms";

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void error_callback(int, const char* description) {
  cout << "glfw error: " << description << endl;
}

void key_callback(GLFWwindow* window, int key, int scan_code, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
}

void cursor_pos_callback(GLFWwindow* window, double, double) {
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
  if (y_offset == 0.0) {
    return;
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

GLuint create_shader(GLenum type, const char *shader_text) {
  auto shader = glCreateShader(type);
  glShaderSource(shader, 1, &shader_text, nullptr);
  glCompileShader(shader);
  return shader;
}

GLuint create_program(GLuint vertex_shader, GLuint fragment_shader) {
  auto program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  return texture;
}
