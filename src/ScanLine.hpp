#pragma once
#include "Camera/Camera.hpp"
#include "Eigen/Core"
#include <array>
#include <cmath>
#include <tuple>
#include <vector>

struct Edge {
  float x;
  float z;
  float maxY;
  float k;
  float dy;
  std::size_t polygonId;
  Edge() = default;
  Edge(int idx, float x, float maxY, float k, float d, float z)
      : x(x), k(k), dy(d), polygonId(idx), z(z), maxY(maxY) {}
};

struct Polygon {
  int id;
  float dzx;
  float dzy;
  float A, B, C, D;
  bool isIn;
  Polygon() = default;
  Polygon(int id, const Eigen::Vector4f &coef)
      : id(id), isIn(false), A(coef[0]), B(coef[1]), C(coef[2]), D(coef[3]) {
    dzx = coef[2] == 0 ? 0 : -coef[0] / coef[2];
    dzy = coef[2] == 0 ? 0 : coef[1] / coef[2];
  }
};

std::vector<std::size_t> _scan(std::vector<std::size_t> &edgeIndexes,
                               std::vector<std::size_t> &polygonIndexes,
                               std::vector<Edge> &edges,
                               std::vector<Polygon> &polygons,
                               std::vector<float> &zbuffer, int y, int width);

struct Primitive {
  Eigen::Vector3f transformed[3];
  Eigen::Vector2i projected[3];
  Eigen::Vector4f coef; // plane coefficient
  size_t num;
  int maxY;
  int minY;
  bool isCoefCalculated;
  Primitive() : num(0), maxY(0), minY(1000), isCoefCalculated(false) {}
  inline void add(const Eigen::Vector4f &nv, const Camera &camera) {
    projected[num] = Eigen::Vector2i(int((nv[0] / nv[2] + 1.0f) * camera.width / 2),
                                     int((nv[1] / nv[2] + 1.0f) * camera.height / 2));
    transformed[num] = Eigen::Vector3f(nv[0], nv[1], nv[2]);
    maxY = maxY > projected[num][1] ? maxY : projected[num][1];
    minY = minY < projected[num][1] ? minY : projected[num][1];
    num++;
  }

  const Eigen::Vector4f &getPlaneCoefficient() {
    if (isCoefCalculated)
      return coef;

    auto &t = transformed;
    auto &p = projected;
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
    coef << A, B, C, D;
    isCoefCalculated = true;
    return coef;
  }
};

struct ProcessedMesh {
  std::vector<Primitive> primitives;
};

struct Scaner {
  size_t width;
  size_t height;
  std::vector<Polygon> polygons;
  std::vector<Edge> edges;
  std::vector<std::vector<size_t>> polygonTable;
  std::vector<std::vector<size_t>> edgeTable;
  std::vector<float> zbuffer;

  Scaner(const Camera &c, size_t n)
      : height(c.height), width(c.width), polygonTable(c.height),
        edgeTable(c.height), zbuffer(c.width, -1000.0f) {
    polygons.reserve(n);
    edges.reserve(n * 3);
  }
  void add(Primitive &primitive) {
    auto y = primitive.maxY >= (int)height ? height - 1 : primitive.maxY;
    auto &coef = primitive.getPlaneCoefficient();

    auto i = polygons.size();
    polygons.push_back(Polygon(i, coef));
    polygonTable[y].push_back(i);

    const auto &p = primitive.projected;
    const auto &t = primitive.transformed;
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
      if (upper[1] < 0) {
        continue;
      }
      auto edge =
          Edge(i, upper[0], upper[1], deltaY <= 0 ? 0 : -(float)deltaX / deltaY,
               deltaY, upperZ);
      if (edge.dy >= 1) {
        auto y = (int)upper[1];
        y = height - 1 < y ? height - 1 : y;
        edgeTable[y].push_back(edges.size());
      }

      if (edge.maxY > height - 1) {
        auto deltaY = edge.maxY - height + 1;
        auto deltaX = edge.k * deltaY;
        auto &polygon = polygons[edge.polygonId];
        edge.x += deltaX;
        edge.z += polygon.dzx * deltaX + polygon.dzy * deltaY;
        edge.dy -= deltaY;
      }

      edges.push_back(edge);
    }
  }

  bool scan(size_t y) {
    auto nothing = false;
    auto &edgeIndexes = edgeTable[y];
    auto &polygonIndexes = polygonTable[y];

    for (int x = 0; x < width; x++) {
      zbuffer[x] = -1000.0f;
    }

    if (edgeIndexes.size() == 0) {
      return nothing;
    }

    auto unfinished =
        _scan(edgeIndexes, polygonIndexes, edges, polygons, zbuffer, y, width);
    if (!unfinished.empty() && y > 0) {
      auto &next = edgeTable[y - 1];
      next.insert(next.end(), unfinished.begin(), unfinished.end());
      auto &nextPolygons = polygonTable[y - 1];
      for (auto &idx : next) {
        auto &edge = edges[idx];
        nextPolygons.push_back(edge.polygonId);
      }
    }

    return nothing;
  }
};