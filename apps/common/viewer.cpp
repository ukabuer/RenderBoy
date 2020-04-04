#define GLFW_INCLUDE_NONE
#include "Application.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <glad/glad.h>
#include <memory>

using namespace std;
using namespace std::chrono;
using namespace RB;

unique_ptr<Application> app;

int main(int argc, const char **argv) {
  if (!glfwInit()) {
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  app.reset(create_application());

  app->process_cmd(argc, argv);

  auto window_size = app->get_size();
  auto width = window_size[0];
  auto height = window_size[1];
  auto window_title = app->get_title();

  auto window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);
  if (window == nullptr) {
    cerr << "Faield to create window." << endl;
    glfwTerminate();
    return 1;
  }

  auto controller = app->get_controller();
  glfwSetWindowUserPointer(window, reinterpret_cast<void *>(controller));
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scan_code,
                                int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      return;
    }
  });
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
    auto raw = glfwGetWindowUserPointer(window);
    auto &control = *(reinterpret_cast<TrackballControl *>(raw));
    control.onMouseMove(static_cast<int>(x), static_cast<int>(y));
  });
  glfwSetScrollCallback(
      window, [](GLFWwindow *window, double x_offset, double y_offset) {
        if (y_offset == 0.0) {
          return;
        }
        auto raw = glfwGetWindowUserPointer(window);
        auto &control = *(reinterpret_cast<TrackballControl *>(raw));
        control.onMouseWheelScroll(static_cast<float>(y_offset));
      });
  glfwSetMouseButtonCallback(
      window, [](GLFWwindow *window, int button, int action, int mods) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        auto raw = glfwGetWindowUserPointer(window);
        auto &control = *(reinterpret_cast<TrackballControl *>(raw));
        if (action == GLFW_PRESS) {
          control.onMouseDown(GLFW_MOUSE_BUTTON_LEFT == button,
                              static_cast<int>(x), static_cast<int>(y));
        } else if (action == GLFW_RELEASE) {
          control.onMouseUp(GLFW_MOUSE_BUTTON_LEFT == button,
                            static_cast<int>(x), static_cast<int>(y));
        }
      });

  glfwMakeContextCurrent(window);

  auto loader = reinterpret_cast<GLADloadproc>(&glfwGetProcAddress);
  if (!gladLoadGLLoader(loader)) {
    cerr << "Failed to load OpenGL." << endl;
    return 1;
  }

  app->prepare();

  auto prev_time = chrono::steady_clock::now();
  while (!glfwWindowShouldClose(window)) {
    auto curr_time = steady_clock::now();
    auto delta = duration_cast<milliseconds>(curr_time - prev_time);

    app->render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}