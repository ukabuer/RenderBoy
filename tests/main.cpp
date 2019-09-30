#include "Camera.hpp"
#include "Frame.hpp"
#include <Eigen/Core>
#include <cmath>
#include <gtest/gtest.h>

using namespace Eigen;

TEST(Camera, SetColor) {
  const auto width = 800u;
  const auto height = 600u;
  auto frame = Frame(width, height);
  auto size = frame.getSize();
  auto &colors = frame.getColors();

  frame.setColor(0, Eigen::Vector3f(1.0f, 0.0f, 0.0f));
  EXPECT_EQ(1.0f, colors[0]);

  frame.setColor(799, 599, Eigen::Vector3f(1.0f, 0.0f, 0.0f));
  EXPECT_EQ(1.0f, colors[size - 1]);

  frame.setColor(400, 300, Eigen::Vector3f(0.0f, 1.0f, 0.0f));
  const auto idx = (400 + 300 * width) * 4;
  EXPECT_EQ(1.0f, colors[idx + 1]);
  EXPECT_EQ(1.0f, colors[idx + 3]);
}

int main(int ac, char *av[]) {
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}