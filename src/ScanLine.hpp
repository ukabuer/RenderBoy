#pragma once
#include "Eigen/Core"
#include <array>
#include <cmath>
#include <vector>

struct Edge {
  float x;
  float z;
  float maxY;
  float k;
  float dy;
  std::size_t polygonId;
  Edge(int idx, float x, float maxY, float k, float d, float z)
      : x(x), k(k), dy(d), polygonId(idx), z(z), maxY(maxY) {}
};

struct Polygon {
  int id;
  float dzx;
  float dzy;
  float A, B, C, D;
  bool isIn;
  Polygon(int id, float dzx, float dzy, float A, float B, float C, float D)
      : id(id), dzx(dzx), dzy(dzy), isIn(false), A(A), B(B), C(C), D(D) {}
};

std::vector<std::size_t> scan(std::vector<std::size_t> &edgeIndexes,
                              std::vector<std::size_t> &polygonIndexes,
                              std::vector<Edge> &edges,
                              std::vector<Polygon> &polygons,
                              std::vector<float> &zbuffer, int y, int width);

struct Primitive {
  Eigen::Vector3f transformed[3];
  Eigen::Vector2i projected[3];
  Eigen::Vector4f coef; // plane coefficient
};

struct ProcessedMesh {
  std::vector<Primitive> primitives;
};