#include "Renderer/RasterizationRenderer.hpp"
#include "Primitives.hpp"
#include "utils.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <array>

using namespace std;
using namespace Eigen;
using vec4f = Vector4f;
using vec3f = Vector3f;

static inline auto orient2d(const array<int, 2> &a, const array<int, 2> &b,
                            const array<int, 2> &c) -> int {
  return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

static inline auto max3(int a, int b, int c) -> int {
  auto tmp = a > b ? a : b;
  return c > tmp ? c : tmp;
}

static inline auto min3(int a, int b, int c) -> int {
  auto tmp = a < b ? a : b;
  return c < tmp ? c : tmp;
}

static void RenderTriangle(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                           const Material &material,
                           const std::vector<Light> &lights, Camera &camera) {
  const auto width = static_cast<int>(camera.getWidth());
  const auto height = static_cast<int>(camera.getHeight());

  // Compute triangle bounding box
  auto minX = min3(v0.screen[0], v1.screen[0], v2.screen[0]);
  auto minY = min3(v0.screen[1], v1.screen[1], v2.screen[1]);
  auto maxX = max3(v0.screen[0], v1.screen[0], v2.screen[0]);
  auto maxY = max3(v0.screen[1], v1.screen[1], v2.screen[1]);

  // Clip against screen bounds
  minX = max(minX, 0);
  minY = max(minY, 0);
  maxX = min(maxX, width - 1);
  maxY = min(maxY, height - 1);

  const auto A01 = v0.screen[1] - v1.screen[1];
  const auto B01 = v1.screen[0] - v0.screen[0];
  const auto A12 = v1.screen[1] - v2.screen[1];
  const auto B12 = v2.screen[0] - v1.screen[0];
  const auto A20 = v2.screen[1] - v0.screen[1];
  const auto B20 = v0.screen[0] - v2.screen[0];

  Vertex p;
  p.screen[0] = minX;
  p.screen[1] = minY;
  auto w0_row = orient2d(v1.screen, v2.screen, p.screen);
  auto w1_row = orient2d(v2.screen, v0.screen, p.screen);
  auto w2_row = orient2d(v0.screen, v1.screen, p.screen);

  // Rasterize
  for (p.screen[1] = minY; p.screen[1] <= maxY; p.screen[1]++) {
    // Determine barycentric coordinates
    auto w0 = w0_row;
    auto w1 = w1_row;
    auto w2 = w2_row;

    const auto sum = w0 + w1 + w2;
    for (p.screen[0] = minX; p.screen[0] <= maxX; p.screen[0]++) {
      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0 && sum != 0) {
        p.depth = -(w0 * v0.depth + w1 * v1.depth + w2 * v2.depth) / sum;
        const auto idx = static_cast<size_t>(p.screen[0] + p.screen[1] * width);
        if (p.depth < -1.f || p.depth > 1.f) {
          continue;
        }
        auto &zbuffer = camera.getFrame().z;
        if (p.depth > zbuffer[idx]) {
          camera.setZ(idx, p.depth);
          p.uv = {(w0 * v0.uv[0] + w1 * v1.uv[0] + w2 * v2.uv[0]) / sum,
                  (w0 * v0.uv[1] + w1 * v1.uv[1] + w2 * v2.uv[1]) / sum};
          p.normals =
              (w0 * v0.normals + w1 * v1.normals + w2 * v2.normals) / sum;
          p.T = (w0 * v0.T + w1 * v1.T + w2 * v2.T) / sum;
          p.B = (w0 * v0.B + w1 * v1.B + w2 * v2.B) / sum;
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

  for (auto &mesh : meshes) {
    auto &geo = mesh.geometry;

    auto &vertices = geo->vertices;
    auto &indices = geo->indices;

    for (auto &vertex : vertices) {
      // transform
      Eigen::Vector4f value =
          matrix * Vector4f(vertex.position[0], vertex.position[1],
                            vertex.position[2], 1.0f);
      vertex.depth = value[2] / value[3];

      // project to screen coords
      vertex.screen = {
          static_cast<int>((value[0] / value[3] + 1.f) * width / 2.0f),
          static_cast<int>((value[1] / value[3] + 1.f) * height / 2.0f)};
    }

    auto &material = *mesh.material;
    const auto triangleNum = indices.size() / 3;
    ParallelForEach(0ull, triangleNum,
                    [&vertices, &indices, &material, &scene, &camera](auto i) {
                      i *= 3;
                      RenderTriangle(vertices[indices[i]],
                                     vertices[indices[i + 1]],
                                     vertices[indices[i + 2]], material,
                                     scene.lights, camera);
                    });
  }
}
