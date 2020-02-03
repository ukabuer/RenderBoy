#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <RenderBoy/Math/Vec.hpp>

using namespace RB;

TEST_CASE("Math", "Vec2") {
  Vec2f a{1.0f, 1.0f};
  Vec2f b{0.0f, -1.0f};
  REQUIRE((a[0] == 1.0f && b[1] == -1.0f));
  REQUIRE((a + b) == Vec2f{1.0f, 0.0f});
}