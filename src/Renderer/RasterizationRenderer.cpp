#include "Renderer/RasterizationRenderer.hpp"
#include "Primitives.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <array>

using namespace std;
using namespace Eigen;
using vec4f = Vector4f;
using vec3f = Vector3f;

inline int orient2d(const Point &a, const Point &b, const Point &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline int max3(int a, int b, int c) {
  auto tmp = a > b ? a : b;
  return c > tmp ? c : tmp;
}

inline int min3(int a, int b, int c) {
  auto tmp = a < b ? a : b;
  return c < tmp ? c : tmp;
}

inline void drawTriangle(const Point &v0, const Point &v1, const Point &v2,
                         const Material &material,
                         const std::vector<std::shared_ptr<PointLight>> &lights,
                         const Eigen::Vector3f &view, Frame &frame) {
  const auto width = static_cast<int>(frame.getWidth());
  const auto height = static_cast<int>(frame.getHeight());

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

  const auto A01 = v0.y - v1.y, B01 = v1.x - v0.x;
  const auto A12 = v1.y - v2.y, B12 = v2.x - v1.x;
  const auto A20 = v2.y - v0.y, B20 = v0.x - v2.x;

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
        if (p.z > frame.zBuffer[idx]) {
          frame.zBuffer[idx] = p.z;
          p.u = (w0 * v0.u + w1 * v1.u + w2 * v2.u) / sum;
          p.v = (w0 * v0.v + w1 * v1.v + w2 * v2.v) / sum;
          p.normals[0] =
              (w0 * v0.normals[0] + w1 * v1.normals[0] + w2 * v2.normals[0]) /
              sum;
          p.normals[1] =
              (w0 * v0.normals[1] + w1 * v1.normals[1] + w2 * v2.normals[1]) /
              sum;
          p.normals[2] =
              (w0 * v0.normals[2] + w1 * v1.normals[2] + w2 * v2.normals[2]) /
              sum;
          p.position[0] = (w0 * v0.position[0] + w1 * v1.position[0] +
                           w2 * v2.position[0]) /
                          sum;
          p.position[1] = (w0 * v0.position[1] + w1 * v1.position[1] +
                           w2 * v2.position[1]) /
                          sum;
          p.position[2] = (w0 * v0.position[2] + w1 * v1.position[2] +
                           w2 * v2.position[2]) /
                          sum;
          const auto color = material.getColor(p, lights, view);
          frame.setColor(idx, color);
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

Frame RasterizationRenderer::render(const Scene &scene, const Camera &camera) {
  auto &meshes = scene.meshes;
  const auto width = camera.getWidth();
  const auto height = camera.getHeight();

  Frame frame(width, height);
  const Vector3f background(0.4f, 0.4f, 0.4f);
  for (auto i = 0ul; i < frame.zBuffer.size(); i++) {
    frame.setColor(i, background);
  }

  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f projectionMatrix = camera.getProjectionMatrix();
  const Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<array<Point, 3>> primitives;
  size_t offset = 0;
  for (auto &mesh : meshes) {
    auto &geo = mesh->geometry;

    auto &indices = geo->indices;
    auto &vertices = geo->vertices;
    auto &texCoords = geo->texCoords;
    auto &normals = geo->normals;

    const auto useIndices = !indices.empty();
    const auto vertexNum = useIndices ? indices.size() : vertices.size();
    const auto primitiveNum = vertexNum / 3;

    primitives.resize(offset + primitiveNum);

    for (size_t j = 0; j < primitiveNum; j++) {
      const auto idx = j * 3;
      assert(offset + j < primitives.size());
      auto &primitive = primitives[offset + j];

      for (size_t k = 0; k < 3; k++) {
        auto &v = useIndices ? vertices[indices[idx + k]] : vertices[idx + k];
        auto uv = Vector2f(0.0f, 0.0f);
        if (!texCoords.empty()) {
          uv = useIndices ? texCoords[indices[idx + k]] : texCoords[idx + k];
        }
        auto normal = Vector3f(0.0f, 0.0f, 0.0f);
        if (!normals.empty()) {
          normal = useIndices ? normals[indices[idx + k]] : normals[idx + k];
        }
        vec4f nv = matrix * vec4f(v[0], v[1], v[2], 1.0);
        const Point p{
            static_cast<int>((nv[0] / nv[3] + 1.0f) * width / 2),
            static_cast<int>((nv[1] / nv[3] + 1.0f) * height / 2),
            nv[2] / nv[3],
            uv[0],
            uv[1],
            {normal[0], normal[1], normal[2]},
            {v[0], v[1], v[2]},
        };
        primitive[k] = p;
      }
    }
    offset += primitiveNum;

    for (auto &p : primitives) {
      drawTriangle(p[0], p[1], p[2], *mesh->material,
                   scene.pointLights, camera.getPosition(), frame);
    }
  }

  return frame;
}
