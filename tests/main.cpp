#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <Eigen/Core>
#include <RenderBoy/Frame.hpp>
#include <cmath>

using namespace Eigen;
using namespace RB;

TEST_CASE("Frame", "SetColor") {
  const auto width = 800;
  const auto height = 600;
  auto frame = Frame(width, height);
  auto size = frame.getSize();
  auto &colors = frame.getColors();

  frame.setColor(0, Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
  REQUIRE(1.0f == colors[0]);

  frame.setColor(799, 599, Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
  REQUIRE(1.0f == colors[size - 1]);

  frame.setColor(400, 300, Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
  const auto idx = (400 + 300 * width) * 4;
  REQUIRE(1.0f == colors[idx + 1]);
  REQUIRE(1.0f == colors[idx + 3]);
}
