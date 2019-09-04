#include "Renderer/RasterizationRenderer.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <array>

using namespace std;
using namespace Eigen;
using vec2i = Eigen::Vector2i;
using vec4f = Eigen::Vector4f;
using vec3f = Eigen::Vector3f;

struct Point2D {
  int x, y;
  float z;
  Point2D() = default;
  Point2D(int x, int y, float z) : x(x), y(y), z(z) {}
};

int orient2d(const Point2D &a, const Point2D &b, const Point2D &c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

inline int max3(int a, int b, int c) {
  int tmp = a > b ? a : b;
  return c > tmp ? c : tmp;
}

inline int min3(int a, int b, int c) {
  int tmp = a < b ? a : b;
  return c < tmp ? c : tmp;
}

inline int max(int a, int b) { return a > b ? a : b; }

inline int min(int a, int b) { return a < b ? a : b; }

inline void drawTri(const Point2D &v0, const Point2D &v1, const Point2D &v2,
                    size_t width, size_t height, Frame &frame) {
  // Compute triangle bounding box
  int minX = min3(v0.x, v1.x, v2.x);
  int minY = min3(v0.y, v1.y, v2.y);
  int maxX = max3(v0.x, v1.x, v2.x);
  int maxY = max3(v0.y, v1.y, v2.y);

  // Clip against screen bounds
  minX = max(minX, 0);
  minY = max(minY, 0);
  maxX = min(maxX, width - 1);
  maxY = min(maxY, height - 1);

  int A01 = v0.y - v1.y, B01 = v1.x - v0.x;
  int A12 = v1.y - v2.y, B12 = v2.x - v1.x;
  int A20 = v2.y - v0.y, B20 = v0.x - v2.x;

  Point2D p = {minX, minY, 0};
  int w0_row = orient2d(v1, v2, p);
  int w1_row = orient2d(v2, v0, p);
  int w2_row = orient2d(v0, v1, p);

  // Rasterize
  for (p.y = minY; p.y <= maxY; p.y++) {
    // Determine barycentric coordinates
    int w0 = w0_row;
    int w1 = w1_row;
    int w2 = w2_row;

    for (p.x = minX; p.x <= maxX; p.x++) {
      // If p is on or inside all edges, render pixel.
      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        auto idx = p.x + p.y * width;
        auto z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
        if (z > frame.zBuffer[idx]) {
          frame.zBuffer[idx] = z;
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

  Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  Matrix4f projectionMatrix = camera.getProjectionMatrix();
  Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<array<Point2D, 3>> primitives;
  for (size_t i = 0; i < meshes.size(); i++) {
    auto &mesh = meshes[i];
    auto &geo = mesh->getGeometry();
    size_t offset = 0;

    auto &indices = geo.getIndicess();
    auto &vertices = geo.getVertices();

    const auto useIndices = indices.size() > 0;
    const auto vertexNum = useIndices ? indices.size() : vertices.size();
    const auto primitiveNum = vertexNum / 3;

    primitives.resize(offset + primitiveNum);

    for (size_t j = 0; j < primitiveNum; j++) {
      const auto idx = j * 3;
      assert((offset + j) < primitives.size());
      auto &primitive = primitives[offset + j];

      for (size_t k = 0; k < 3; k++) {
        auto &v = useIndices ? vertices[indices[idx + k]] : vertices[idx + k];
        vec4f nv = matrix * vec4f(v[0], v[1], v[2], 1.0);
        Point2D p(int((nv[0] / nv[2] + 1.0f) * width / 2),
                  int((nv[1] / nv[2] + 1.0f) * height / 2), nv[3]);
        primitive[k] = p;
      }
    }
  }

  for (auto &p : primitives) {
    drawTri(p[0], p[1], p[2], width, height, frame);
  }

  auto total = width * height;
  auto maxZ = FLT_MIN;
  for (size_t i = 0; i < total; i++) {
    auto z = frame.zBuffer[i];
    if (z > maxZ) {
      maxZ = z;
    }
  }

  auto idx = 0;
  for (size_t y = 0; y < height; y++) {
    for (size_t x = 0; x < width; x++) {
      if (frame.zBuffer[idx] == FLT_MIN) {
        idx++;
        continue;
      }
      auto offset = idx * 4;
      auto z = frame.zBuffer[idx] / maxZ;
      frame.colors[offset] = z;
      frame.colors[offset + 1] = frame.colors[offset + 2] = z;
      frame.colors[offset + 3] = 1.0f;
      idx++;
    }
  }

  return frame;
}