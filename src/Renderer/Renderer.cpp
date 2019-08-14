#include "Renderer/Renderer.hpp"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "ScanLine.hpp"
#include "tbb/parallel_for.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <cmath>

using namespace std;
using namespace tbb;
using vec3f = Eigen::Vector3f;
using vec4f = Eigen::Vector4f;
using vec2i = Eigen::Vector2i;
using Eigen::Matrix3f;
using Eigen::Matrix4f;

Frame Renderer::render(const Scene &scene, const Camera &camera) {
  const auto &meshes = scene.meshes;
  Frame frame(camera.width, camera.height);

  Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  Matrix4f projectionMatrix = camera.getProjectionMatrix();
  Matrix4f matrix = projectionMatrix * viewMatrix;

  vector<Polygon> polygons;
  vector<Edge> edges;

  // build table
  vector<vector<size_t>> polygonTable(camera.height);
  vector<vector<size_t>> edgeTable(camera.height);

  vector<ProcessedMesh> processeds(meshes.size());
  for (int i = 0; i < meshes.size(); i++) {
    auto &mesh = meshes[i];
    auto &geo = mesh->geometry;
    auto &processed = processeds[i];
    auto &primitives = processed.primitives;

    const auto useIndices = geo->indices.size() > 0;
    const auto vertexNum =
        useIndices ? geo->indices.size() : geo->vertices.size();
    primitives.resize(vertexNum / 3);

    parallel_for(blocked_range<size_t>(0, primitives.size()), [&](auto &r) {
      for (auto i = r.begin(); i != r.end(); i++) {
        auto startIdx = i * 3;
        auto &primitive = primitives[i];
        auto &t = primitive.transformed;
        auto &p = primitive.projected;
        int maxY = -1000;
        int minY = 1000;

        for (int j = 0; j < 3; j++) {
          auto &v = useIndices ? geo->vertices[geo->indices[startIdx + j]]
                               : geo->vertices[startIdx + j];
          vec4f nv = matrix * vec4f(v[0], v[1], v[2], 1.0);
          p[j] = vec2i((nv[0] + 1.0f) * camera.width / 2,
                       (nv[1] + 1.0f) * camera.height / 2);
          t[j] = vec3f(nv[0], nv[1], nv[2]);
          maxY = maxY > p[j][1] ? maxY : p[j][1];
          minY = minY < p[j][1] ? minY : p[j][1];
        }

        if (maxY < 0 || minY >= camera.height) {
          continue;
        }

        float A, B, C, D;
        if (t[0][2] == t[1][2] && t[1][2] == t[2][2]) {
          A = 0, B = 0, C = 1, D = t[0][2];
        } else {
          // A(x - x1) + B(y - y1) + C(z - z1) = 0
          // A = (y2 - y1)*(z3 - z1) - (z2 -z1)*(y3 - y1)
          A = (p[1][1] - p[0][1]) * (t[2][2] - t[0][2]) -
                    (t[1][2] - t[0][2]) * (p[2][1] - p[0][1]);
          // B = (x3 - x1)*(z2 - z1) - (x2 - x1)*(z3 - z1)
          B = (p[2][0] - p[0][0]) * (t[1][2] - t[0][2]) -
                    (p[1][0] - p[0][0]) * (t[2][2] - t[0][2]);
          // C = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1)
          C = (float)((p[1][0] - p[0][0]) * (p[2][1] - p[0][1]) -
                            (p[2][0] - p[0][0]) * (p[1][1] - p[0][1]));
          D = -A * p[0][0] - B * p[0][1] - C * t[0][2];
        }

        auto y = maxY >= (int)camera.height ? camera.height - 1 : maxY;
        auto dzx = C == 0 ? 0 : -A / C;
        auto dzy = C == 0 ? 0 : B / C;

        polygons.push_back(Polygon(i, dzx, dzy, A, B, C, D));
        polygonTable[y].push_back(i);
        const auto num = 3;
        for (int j = 0; j < num; j++) {
          auto index = polygonTable[y].back();
          auto &polygon = polygons[index];

          auto &p1 = p[j];
          auto &p2 = p[(j + 1) % num];
          auto upperIdx = p1[1] > p2[1] ? j : (j + 1) % num;
          auto lowerIdx = p1[1] > p2[1] ? (j + 1) % num : j;

          auto &upper = p1[1] > p2[1] ? p1 : p2;
          auto &lower = p1[1] > p2[1] ? p2 : p1;
          auto deltaX = upper[0] - lower[0];
          auto deltaY = upper[1] - lower[1];
          auto upperZ = t[upperIdx][2];
          auto edge =
              Edge(i, upper[0], upper[1],
                   deltaY <= 0 ? 0 : -(float)deltaX / deltaY, deltaY, upperZ);
          edges.push_back(edge);
          if (edge.dy >= 1) {
            auto y = (int)upper[1];
            y = min((int)camera.height - 1, y);
            edgeTable[y].push_back(edges.size() - 1);
          }
        }
      }
    });

    vector<float> zbuffer(camera.width, -1000.0f);
    for (auto &edge : edges) {
      if (edge.maxY > camera.height - 1) {
        auto deltaY = edge.maxY - camera.height + 1;
        auto deltaX = edge.k * deltaY;
        auto &polygon = polygons[edge.polygonId];
        edge.x += deltaX;
        edge.z += polygon.dzx * deltaX + polygon.dzy * deltaY;
        edge.dy -= deltaY;
      }
    }

    for (int y = camera.height - 1; y >= 0; y--) {
      auto &edgeIndexes = edgeTable[y];
      auto &polygonIndexes = polygonTable[y];

      if (edgeIndexes.size() == 0) {
        continue;
      }

      auto unfinished = scan(edgeIndexes, polygonIndexes, edges, polygons,
                             zbuffer, y, camera.width);
      if (!unfinished.empty() && y > 0) {
        auto &next = edgeTable[y - 1];
        next.insert(next.end(), unfinished.begin(), unfinished.end());
        auto &nextPolygons = polygonTable[y - 1];
        for (auto &idx : next) {
          auto &edge = edges[idx];
          nextPolygons.push_back(edge.polygonId);
        }
      }

      for (int x = 0; x < camera.width; x++) {
        int offset = (x + (camera.height - y - 1) * camera.width) * 4;
        float v = 0;
        if (zbuffer[x] != -1000.0f) {
          v = 1.0f;
        }
        frame.colors[offset] = v;
        frame.colors[offset + 1] = frame.colors[offset + 2] = 0;
        frame.colors[offset + 3] = 1.0f;
        zbuffer[x] = -1000.0f;
      }
    }
  }

  return frame;
}