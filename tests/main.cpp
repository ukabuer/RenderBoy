#include <gtest/gtest.h>
#include "Frame.hpp"

TEST(Frame, SetColor) {
  const auto width = 800u;
  const auto height = 600u;
  auto frame = Frame(width, height);

  frame.setColor(0, Eigen::Vector3f(1.0f, 0.0f, 0.0f));
  EXPECT_EQ(1.0f, frame.colors[0]);

  frame.setColor(799, 599, Eigen::Vector3f(1.0f, 0.0f, 0.0f));
  EXPECT_EQ(1.0f, frame.colors[frame.colors.size() - 1]);

  frame.setColor(400, 300, Eigen::Vector3f(0.0f, 1.0f, 0.0f));
  const auto idx = (400 + 300 * width) * 4;
  EXPECT_EQ(1.0f, frame.colors[idx + 1]);
  EXPECT_EQ(1.0f, frame.colors[idx + 3]);
}

int main(int ac, char* av[]) {
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}