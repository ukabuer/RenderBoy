#include "Renderer/RasterizationRenderer.hpp"
#include <Eigen/Core>
#include <array>
#include "Primitives.hpp"

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

inline int max(int a, int b) { return a > b ? a : b; }

inline int min(int a, int b) { return a < b ? a : b; }

inline void drawTriangle(const Point &v0, const Point &v1, const Point &v2,
                         const Material &material,
                         int width, int height, Frame &frame) {
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
        if (p.z > frame.zBuffer[idx]) {
          frame.zBuffer[idx] = p.z;
          p.u = (w0 * v0.u + w1 * v1.u + w2 * v2.u) / sum;
          p.v = (w0 * v0.v + w1 * v1.v + w2 * v2.v) / sum;
          const auto color = material.getColor(p);
          frame.setColor(idx, color[0], color[1], color[2], color[3]);
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
  auto meshes = scene.getMeshes();
  const auto width = camera.getWidth();
  const auto height = camera.getHeight();

  Frame frame(width, height);

  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f projectionMatrix = camera.getProjectionMatrix();
  const Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<array<Point, 3>> primitives;
  size_t offset = 0;
  for (auto &mesh : meshes) {
    auto &geo = mesh->getGeometry();

    auto &indices = geo.getIndices();
    auto &vertices = geo.getVertices();
    auto &texCoords = geo.getTexCoords();
    auto &normals = geo.getNormals();

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
        auto &uv =
            useIndices ? texCoords[indices[idx + k]] : texCoords[idx + k];
        // auto normal = useIndices ? normals[indices[idx + k]] : normals[idx + k];
        vec4f nv = matrix * vec4f(v[0], v[1], v[2], 1.0);
        const Point p{static_cast<int>((nv[0] / nv[3] + 1.0f) * width / 2),
                      static_cast<int>((nv[1] / nv[3] + 1.0f) * height / 2),
                      nv[2] / nv[3],
                      uv[0],
                      uv[1]};
        primitive[k] = p;
      }
    }
    offset += primitiveNum;

    for (auto &p : primitives) {
      drawTriangle(p[0], p[1], p[2], mesh->getMaterial(), width, height, frame);
    }
  }

  return frame;
}
