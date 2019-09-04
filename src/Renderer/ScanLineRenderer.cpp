#include "Renderer/ScanLineRenderer.hpp"
#include "ScanLine.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <algorithm>
#include <cmath>
#include <tbb/parallel_for.h>
#include <utility>
#include <vector>

using namespace std;
using namespace tbb;
using vec3f = Eigen::Vector3f;
using vec4f = Eigen::Vector4f;
using vec2i = Eigen::Vector2i;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

Frame ScanLineRenderer::render(const Scene &scene, const Camera &camera) {
  auto meshes = scene.getMeshes();
  const auto width = camera.getWidth();
  const auto height = camera.getHeight();

  Frame frame(width, height);

  Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  Matrix4f projectionMatrix = camera.getProjectionMatrix();
  Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<Primitive> primitives;
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

    parallel_for(blocked_range<size_t>(0, primitiveNum), [&](auto &r) {
      for (size_t j = r.begin(); j < r.end(); j++) {
        const auto idx = j * 3;
        assert((offset + j) < primitives.size());
        auto &primitive = primitives[offset + j];

        for (size_t k = 0; k < 3; k++) {
          auto &v = useIndices ? vertices[indices[idx + k]] : vertices[idx + k];
          vec4f out = matrix * vec4f(v[0], v[1], v[2], 1.0);
          primitive.add(out, camera);
        }
      }
    });
  }

  Scaner scaner(width, height, primitives.size());
  for (auto &primitive : primitives) {
    if (primitive.maxY < 0 || primitive.minY >= height) {
      continue;
    }
    scaner.add(primitive);
  }

  for (int y = height - 1; y >= 0; y--) {
    if (!scaner.scan(y)) {
      continue;
    }

    for (int x = 0; x < width; x++) {
      int offset = (x + (height - y - 1) * width) * 4;
      float v = 0;
      if (scaner.zbuffer[x] != FLT_MIN) {
        v = 1.0f;
      }
      frame.colors[offset] = v;
      frame.colors[offset + 1] = frame.colors[offset + 2] = 0;
      frame.colors[offset + 3] = 1.0f;
    }
  }

  return frame;
}