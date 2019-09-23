#include "Renderer/RasterizationRenderer.hpp"
#include "Primitives.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <array>
#include "utils.hpp"

using namespace std;
using namespace Eigen;
using vec4f = Vector4f;
using vec3f = Vector3f;

static inline auto orient2d(const Point &a, const Point &b, const Point &c)
    -> int {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static inline auto max3(int a, int b, int c) -> int {
  auto tmp = a > b ? a : b;
  return c > tmp ? c : tmp;
}

static inline auto min3(int a, int b, int c) -> int {
  auto tmp = a < b ? a : b;
  return c < tmp ? c : tmp;
}

static void RenderTriangle(const Point &v0, const Point &v1, const Point &v2,
                           const Material &material,
                           const std::vector<Light> &lights, Camera &camera) {
  const auto width = static_cast<int>(camera.getWidth());
  const auto height = static_cast<int>(camera.getHeight());

  // Compute triangle bounding box
  auto minX = min3(v0.x, v1.x, v2.x);
  auto minY = min3(v0.y, v1.y, v2.y);
  auto maxX = max3(v0.x, v1.x, v2.x);
  auto maxY = max3(v0.y, v1.y, v2.y);

  // Clip against screen bounds
  minX = max(minX, 0);
  minY = max(minY, 0);
  maxX = min(maxX, width - 1);
  maxY = min(maxY, height - 1);

  const auto A01 = v0.y - v1.y;
  const auto B01 = v1.x - v0.x;
  const auto A12 = v1.y - v2.y;
  const auto B12 = v2.x - v1.x;
  const auto A20 = v2.y - v0.y;
  const auto B20 = v0.x - v2.x;

  Point p;
  p.x = minX;
  p.y = minY;
  auto w0_row = orient2d(v1, v2, p);
  auto w1_row = orient2d(v2, v0, p);
  auto w2_row = orient2d(v0, v1, p);

  // Rasterize
  for (p.y = minY; p.y <= maxY; p.y++) {
    // Determine barycentric coordinates
    auto w0 = w0_row;
    auto w1 = w1_row;
    auto w2 = w2_row;

    const auto sum = w0 + w1 + w2;
    for (p.x = minX; p.x <= maxX; p.x++) {
      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0 && sum != 0) {
        p.z = -(w0 * v0.z + w1 * v1.z + w2 * v2.z) / sum;
        const auto idx = static_cast<size_t>(p.x + p.y * width);
        if (p.z < -1.f || p.z > 1.f) {
          continue;
        }
        auto &zbuffer = camera.getFrame().z;
        if (p.z > zbuffer[idx]) {
          camera.setZ(idx, p.z);
          p.uv = (w0 * v0.uv + w1 * v1.uv + w2 * v2.uv) / sum;
          p.normals =
              (w0 * v0.normals + w1 * v1.normals + w2 * v2.normals) / sum;
          p.position =
              (w0 * v0.position + w1 * v1.position + w2 * v2.position) / sum;
          const auto color = material.sample(p, lights, camera);
          camera.setColor(idx, color);
        }
      }

      w0 += A12;
      w1 += A20;
      w2 += A01;
    }
    w0_row += B12;
    w1_row += B20;
    w2_row += B01;
  }
}

void RasterizationRenderer::render(const Scene &scene, Camera &camera) {
  auto &meshes = scene.meshes;
  const auto width = camera.getWidth();
  const auto height = camera.getHeight();

  // clear frame
  camera.clearFrame(scene.background);

  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f projectionMatrix = camera.getProjectionMatrix();
  const Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<array<Point, 3>> primitives;
  size_t offset = 0;
  for (auto &mesh : meshes) {
    auto &geo = mesh.geometry;

    auto &indices = geo->indices;
    auto &vertices = geo->vertices;
    auto &texCoords = geo->texCoords;
    auto &normals = geo->normals;

    const auto vertexNum = indices.size();
    const auto primitiveNum = vertexNum / 3;

    primitives.resize(offset + primitiveNum);

    for (size_t j = 0; j < primitiveNum; j++) {
      const auto idx = j * 3;
      assert(offset + j < primitives.size());
      auto &primitive = primitives[offset + j];

      for (size_t k = 0; k < 3; k++) {
        auto &v = vertices[indices[idx + k]];
        auto uv = Vector2f(0.0f, 0.0f);
        if (!texCoords.empty()) {
          uv = texCoords[indices[idx + k]];
        }
        auto normal = Vector3f(0.0f, 0.0f, 0.0f);
        if (!normals.empty()) {
          normal = normals[indices[idx + k]];
        }
        vec4f nv = matrix * vec4f(v[0], v[1], v[2], 1.0);
        const Point p{
            static_cast<int>((nv[0] / nv[3] + 1.0f) * width / 2),
            static_cast<int>((nv[1] / nv[3] + 1.0f) * height / 2),
            nv[2] / nv[3],
            uv,
            normal,
            v,
        };
        primitive[k] = p;
      }
    }
    offset += primitiveNum;

    ParallelForEach(0ull, primitives.size(), [&mesh, &scene, &camera, &primitives](auto i) {
      const auto &p = primitives[i];
      RenderTriangle(p[0], p[1], p[2], *mesh.material, scene.lights, camera);
    });
  }
}
