#include "Renderer/Renderer.hpp"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "ScanLine.hpp"
#include "tbb/parallel_for.h"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

using namespace std;
using namespace tbb;
using vec3f = Eigen::Vector3f;
using vec4f = Eigen::Vector4f;
using vec2i = Eigen::Vector2i;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

Frame Renderer::render(const Scene &scene, const Camera &camera) {
  const auto &meshes = scene.meshes;
  const auto width = camera.getWidth();
  const auto height = camera.getHeight();

  Frame frame(width, height);

  Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  Matrix4f projectionMatrix = camera.getProjectionMatrix();
  Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<ProcessedMesh> processeds(meshes.size());
  for (int i = 0; i < meshes.size(); i++) {
    auto &mesh = meshes[i];
    auto &geo = mesh->geometry;
    auto &primitives = processeds[i].primitives;

    const auto useIndices = geo->indices.size() > 0;
    const auto vertexNum =
        useIndices ? geo->indices.size() : geo->vertices.size();
    primitives.resize(vertexNum / 3);

    Scaner scaner(width, height, primitives.size());

    // parallel_for(blocked_range<size_t>(0, primitives.size()), [&](auto &r) {
    for (auto i = 0; i != primitives.size(); i++) {
      auto idx = i * 3;
      auto &primitive = primitives[i];

      for (int j = 0; j < 3; j++) {
        auto &v = useIndices ? geo->vertices[geo->indices[idx + j]]
                             : geo->vertices[idx + j];
        vec4f out = matrix * vec4f(v[0], v[1], v[2], 1.0);
        primitive.add(out, camera);
      }

      if (primitive.maxY < 0 || primitive.minY >= height) {
        continue;
      }

      scaner.add(primitive);
    }
    // });

    for (int y = height - 1; y >= 0; y--) {
      auto nothing = scaner.scan(y);

      if (nothing) {
        continue;
      }

      for (int x = 0; x < width; x++) {
        int offset = (x + (height - y - 1) * width) * 4;
        float v = 0;
        if (scaner.zbuffer[x] != -1000.0f) {
          v = 1.0f;
        }
        frame.colors[offset] = v;
        frame.colors[offset + 1] = frame.colors[offset + 2] = 0;
        frame.colors[offset + 3] = 1.0f;
      }
    }
  }

  return frame;
}