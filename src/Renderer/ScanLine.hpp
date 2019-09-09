#pragma once

#include "Camera/Camera.hpp"
#include <Eigen/Core>
#include <vector>

struct ScanEdge {
  size_t polygonId = 0;
  float x = 0.0f;
  float maxY = 0.0f;
  float k = 0.0f;
  float dy = 0.0f;
  float z = 0.0f;
};

struct ScanPolygon {
  size_t id;
  float dzx;
  float dzy;
  float A, B, C, D;
  bool isIn;
  ScanPolygon() = default;

  ScanPolygon(size_t id, const Eigen::Vector4f &coef)
    : id(id), A(coef[0]), B(coef[1]), C(coef[2]), D(coef[3]), isIn(false) {
    dzx = coef[2] == 0 ? 0 : -coef[0] / coef[2];
    dzy = coef[2] == 0 ? 0 : coef[1] / coef[2];
  }
};

struct ScanPrimitive {
  Eigen::Vector3f transformed[3];
  Eigen::Vector2i projected[3];
  Eigen::Vector4f coef; // plane coefficient
  size_t num;
  int maxY;
  int minY;
  mutable bool isCoefCalculated;

  ScanPrimitive()
    : num(0), maxY(0), minY(1000), isCoefCalculated(false) {
  }

  void add(const Eigen::Vector4f &nv, const Camera &camera) {
    const auto width = camera.getWidth();
    const auto height = camera.getHeight();
    projected[num] = Eigen::Vector2i(int((nv[0] / nv[2] + 1.0f) * width / 2),
                                     int((nv[1] / nv[2] + 1.0f) * height / 2));
    transformed[num] = Eigen::Vector3f(nv[0], nv[1], nv[2]);
    maxY = maxY > projected[num][1] ? maxY : projected[num][1];
    minY = minY < projected[num][1] ? minY : projected[num][1];
    num++;
  }

  const Eigen::Vector4f &getPlaneCoefficient() const {
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
      A = static_cast<float>(p[1][1] - p[0][1]) * (t[2][2] - t[0][2]) -
          static_cast<float>(p[2][1] - p[0][1]) * (t[1][2] - t[0][2]);
      // B = (x3 - x1)*(z2 - z1) - (x2 - x1)*(z3 - z1)
      B = static_cast<float>(p[2][0] - p[0][0]) * (t[1][2] - t[0][2]) -
          static_cast<float>(p[1][0] - p[0][0]) * (t[2][2] - t[0][2]);
      // C = (x2 - x1)*(y3 - y1) - (x3 - x1)*(y2 - y1)
      C = static_cast<float>((p[1][0] - p[0][0]) * (p[2][1] - p[0][1]) -
                             (p[2][0] - p[0][0]) * (p[1][1] - p[0][1]));
      D = -A * static_cast<float>(p[0][0]) - B * static_cast<float>(p[0][1]) - C
          * t[0][2];
    }
    auto &coef = const_cast<Eigen::Vector4f &>(this->coef);
    coef << A, B, C, D;
    isCoefCalculated = true;
    return coef;
  }
};

struct ProcessedMesh {
  std::vector<ScanPrimitive> primitives;
};

struct Scaner {
  size_t width;
  size_t height;
  std::vector<ScanPolygon> polygons;
  std::vector<ScanEdge> edges;
  std::vector<std::vector<size_t>> polygonTable;
  std::vector<std::vector<size_t>> edgeTable;
  std::vector<float> zbuffer;

  Scaner(int width, int height, size_t n)
    : width(width), height(height), polygonTable(height), edgeTable(height),
      zbuffer(width, FLT_MIN) {
    polygons.reserve(n);
    edges.reserve(n * 3);
  }

  void add(ScanPrimitive &primitive);
  bool scan(size_t y);
};
