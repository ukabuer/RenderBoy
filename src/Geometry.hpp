#pragma once
#include <Eigen/Core>
#include <vector>

const auto PI = atan(1.0f) * 4.0f;

struct Geometry {
  std::vector<uint32_t> indices;
  std::vector<Eigen::Vector3f> vertices;
  std::vector<Eigen::Vector3f> normals;
  std::vector<Eigen::Vector2f> texCoords;

  static auto Box(float width = 1.0f, float height = 1.0f, float depth = 1.0f)
      -> std::unique_ptr<Geometry>;

  static auto
  Sphere(float radius, uint32_t subdivisionsAxis, uint32_t subdivisionsHeight,
         float startLatitudeInRadians = 0, float endLatitudeInRadians = PI,
         float startLongitudeInRadians = 0,
         float endLongitudeInRadians = 2 * PI) -> std::unique_ptr<Geometry>;
};
