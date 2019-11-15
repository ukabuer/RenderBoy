#include "Renderer/RasterizationRenderer.hpp"
#include "Primitives.hpp"
#include "utils.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <array>
#include <functional>

using namespace std;
using namespace Eigen;
using Shader = function<bool(const Vertex &, Vector3f &)>;

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

static void TraverseTriangle(const Vertex &v0, const Vertex &v1,
                             const Vertex &v2, Frame &frame, Shader shader) {
  const auto width = static_cast<int>(frame.getWidth());
  const auto height = static_cast<int>(frame.getHeight());

  // compute bounding box and clip against screen bounds
  auto minX = max(min3(v0.screen[0], v1.screen[0], v2.screen[0]), 0);
  auto minY = max(min3(v0.screen[1], v1.screen[1], v2.screen[1]), 0);
  auto maxX = min(max3(v0.screen[0], v1.screen[0], v2.screen[0]), width - 1);
  auto maxY = min(max3(v0.screen[1], v1.screen[1], v2.screen[1]), height - 1);

  // increment for weight on rows and columns
  const Vector3i A = {v1.screen[1] - v2.screen[1], v2.screen[1] - v0.screen[1],
                      v0.screen[1] - v1.screen[1]};
  const Vector3i B = {v2.screen[0] - v1.screen[0], v0.screen[0] - v2.screen[0],
                      v1.screen[0] - v0.screen[0]};

  Vertex p;
  p.screen[0] = minX;
  p.screen[1] = minY;

  Vector3i rowWeight = {orient2d(v1.screen, v2.screen, p.screen),
                        orient2d(v2.screen, v0.screen, p.screen),
                        orient2d(v0.screen, v1.screen, p.screen)};

  for (p.screen[1] = minY; p.screen[1] <= maxY; p.screen[1]++, rowWeight += B) {
    Vector3i weight = rowWeight;

    const auto sum = weight.sum();
    for (p.screen[0] = minX; p.screen[0] <= maxX; p.screen[0]++, weight += A) {
      if (weight[0] < 0 || weight[1] < 0 || weight[2] < 0 || sum == 0) {
        continue;
      }

      Vector3f clip = {static_cast<float>(weight[0]) / (v0.homo * sum),
                       static_cast<float>(weight[1]) / (v1.homo * sum),
                       static_cast<float>(weight[2]) / (v2.homo * sum)};
      clip = clip / clip.sum();

      p.depth = -(clip[0] * v0.depth + clip[1] * v1.depth + clip[2] * v2.depth);
      if (p.depth < -1.f || p.depth > 1.f) {
        continue;
      }

      const auto idx = static_cast<size_t>(p.screen[0] + p.screen[1] * width);

      if (p.depth <= frame.getZ(idx)) {
        continue;
      }
      frame.setZ(idx, p.depth);

      // interpolates, maybe we can use SIMD
      p.position = (clip[0] * v0.position + clip[1] * v1.position +
                    clip[2] * v2.position);
      p.normals =
          (clip[0] * v0.normals + clip[1] * v1.normals + clip[2] * v2.normals);
      p.uv = {(clip[0] * v0.uv[0] + clip[1] * v1.uv[0] + clip[2] * v2.uv[0]),
              (clip[0] * v0.uv[1] + clip[1] * v1.uv[1] + clip[2] * v2.uv[1])};
      p.T = (clip[0] * v0.T + clip[1] * v1.T + clip[2] * v2.T);
      p.B = (clip[0] * v0.B + clip[1] * v1.B + clip[2] * v2.B);

      auto color = Vector3f();
      auto discard = shader(p, color);
      if (discard) {
        continue;
      }
      frame.setColor(idx, color);
    }
  }
}

void RasterizationRenderer::render(const Scene &scene, Camera &camera) {
  auto &meshes = scene.meshes;
  auto &frame = camera.getFrame();
  const auto width = frame.getWidth();
  const auto height = frame.getHeight();

  // clear frame
  frame.clear(scene.background);

  // no model transform now
  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f projectionMatrix = camera.getProjectionMatrix();
  const Matrix4f matrix = projectionMatrix * viewMatrix;

  for (auto &mesh : meshes) {
    auto &geometry = mesh.geometry;
    auto &vertices = geometry->vertices;
    auto &indices = geometry->indices;

    for (auto &vertex : vertices) {
      // projection
      Vector4f value = matrix * Vector4f(vertex.position[0], vertex.position[1],
                                         vertex.position[2], 1.0f);
      vertex.depth = value[2] / value[3];

      // a little ugly...
      vertex.homo = value[3];

      // map to screen coords
      vertex.screen = {
          static_cast<int>((value[0] / value[3] + 1.f) * width / 2.0f),
          static_cast<int>((value[1] / value[3] + 1.f) * height / 2.0f)};
    }

    const auto shader = [&mesh, &scene, &camera](const Vertex &p,
                                                 Vector3f &color) {
      color = mesh.material->sample(p, scene.lights, camera);
      return false;
    };

    ParallelForEach(0UL, indices.size() / 3,
                    [&vertices, &indices, &shader, &frame](auto i) {
                      i *= 3;
                      TraverseTriangle(
                          vertices[indices[i]], vertices[indices[i + 1]],
                          vertices[indices[i + 2]], frame, shader);
                    });
  }
}
