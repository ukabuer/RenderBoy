#include <gtest/gtest.h>
#include "Frame.hpp"

TEST(Frame, SetColor) {
  const auto width = 800u;
  const auto height = 600u;
  auto frame = Frame(width, height);

  frame.setColor(0, 255, 0, 0, 0);
  EXPECT_EQ(255, frame.colors[0]);

  frame.setColor(799, 599, 0, 0, 0, 255);
  EXPECT_EQ(0xFF, frame.colors[frame.colors.size() - 1]);

  frame.setColor(400, 300, 0, 255, 0, 255);
  const auto idx = (400 + 300 * width) * 4;
  EXPECT_EQ(255, frame.colors[idx + 1]);
  EXPECT_EQ(255, frame.colors[idx + 3]);
}

int main(int ac, char* av[]) {
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}