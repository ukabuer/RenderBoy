#include "Geometry.hpp"

using namespace std;
using namespace Eigen;
using vec3 = Eigen::Vector3f;
using vec2 = Eigen::Vector2f;

auto Geometry::Box(float w, float h, float d) -> unique_ptr<Geometry> {
  static const array<array<uint32_t, 4>, 6> CUBE_FACE_INDICES = {
      {{{3, 7, 5, 1}},
       {{6, 2, 0, 4}},
       {{6, 7, 3, 2}},
       {{0, 1, 5, 4}},
       {{7, 6, 4, 5}},
       {{2, 3, 1, 0}}},
  };
  const auto hw = w / 2;
  const auto hh = h / 2;
  const auto hd = d / 2;
  vector<Vector3f> cornerVertices = {
      vec3(-hw, -hh, -hd), vec3(hw, -hh, -hd), vec3(-hw, hh, -hd),
      vec3(hw, hh, -hd),   vec3(-hw, -hh, hd), vec3(hw, -hh, hd),
      vec3(-hw, hh, hd),   vec3(hw, hh, hd),
  };
  static vector<Vector3f> faceNormals = {
      vec3(1.f, 0.f, 0.f),  vec3(-1.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f),
      vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, 1.f),  vec3(0.f, 0.f, -1.f),
  };
  static vector<Vector2f> uvCoords = {
      vec2(1.f, 0.f),
      vec2(0.f, 0.f),
      vec2(0.f, 1.f),
      vec2(1.f, 1.f),
  };

  const auto numVertices = 4 * 6;
  auto vertices = vector<Vertex>(numVertices);
  auto indices = vector<uint32_t>(3 * 6 * 2);

  auto idx = 0;
  for (auto f = 0u; f < 6; f++) {
    const auto faceIndices = CUBE_FACE_INDICES.at(f);
    for (auto v = 0u; v < 4; v++) {
      auto &vertex = vertices[idx];

      const auto position = cornerVertices[faceIndices.at(v)];
      const auto normals = faceNormals[f];
      const auto uv = uvCoords[v];

      vertex.position = position;
      vertex.normals = normals;
      vertex.uv = {uv[0], uv[1]};
      idx += 1;
    }

    const auto offset = 4 * f;
    indices.insert(indices.end(), {offset + 0, offset + 1, offset + 2});
    indices.insert(indices.end(), {offset + 0, offset + 2, offset + 3});
  }

  auto geometry = make_unique<Geometry>();
  geometry->indices = move(indices);
  geometry->vertices = move(vertices);
  return geometry;
}

auto Geometry::Sphere(float radius, uint32_t subdivisionsAxis,
                      uint32_t subdivisionsHeight, float startLatitudeInRadians,
                      float endLatitudeInRadians, float startLongitudeInRadians,
                      float endLongitudeInRadians) -> unique_ptr<Geometry> {
  const auto latRange = endLatitudeInRadians - startLatitudeInRadians;
  const auto longRange = endLongitudeInRadians - startLongitudeInRadians;

  const auto numVertices = (subdivisionsAxis + 1) * (subdivisionsHeight + 1);
  auto vertices = vector<Vertex>(numVertices);

  auto idx = 0u;
  for (auto y = 0u; y <= subdivisionsHeight; y++) {
    for (auto x = 0u; x <= subdivisionsAxis; x++) {
      auto &vertex = vertices[idx];
      const auto u = static_cast<float>(x) / subdivisionsAxis;
      const auto v = static_cast<float>(y) / subdivisionsHeight;
      const auto theta = longRange * u + startLongitudeInRadians;
      const auto phi = latRange * v + startLatitudeInRadians;
      const auto sinTheta = std::sin(theta);
      const auto cosTheta = std::cos(theta);
      const auto sinPhi = std::sin(phi);
      const auto cosPhi = std::cos(phi);
      const auto ux = cosTheta * sinPhi;
      const auto uy = cosPhi;
      const auto uz = sinTheta * sinPhi;
      vertex = {
          {radius * ux, radius * uy, radius * uz}, // position
          {ux, uy, uz},                            // normals
          {1 - u, v},                              // uv
      };
      idx++;
    }
  }

  const auto numVertsAround = subdivisionsAxis + 1;
  auto indices = vector<uint32_t>();
  indices.reserve(3 * subdivisionsAxis * subdivisionsHeight);
  for (auto x = 0u; x < subdivisionsAxis; x++) {
    for (auto y = 0u; y < subdivisionsHeight; y++) {
      // Make triangle 1 of quad.
      indices.insert(indices.end(), {(y + 0) * numVertsAround + x,
                                     (y + 0) * numVertsAround + x + 1,
                                     (y + 1) * numVertsAround + x});

      // Make triangle 2 of quad.
      indices.insert(indices.end(), {(y + 1) * numVertsAround + x,
                                     (y + 0) * numVertsAround + x + 1,
                                     (y + 1) * numVertsAround + x + 1});
    }
  }

  auto geometry = make_unique<Geometry>();
  geometry->indices = move(indices);
  geometry->vertices = move(vertices);
  return geometry;
}
