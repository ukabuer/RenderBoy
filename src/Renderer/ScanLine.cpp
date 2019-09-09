#include "ScanLine.hpp"

using namespace std;

void Scaner::add(ScanPrimitive &primitive) {
  auto y = primitive.maxY >= (int)height ? height - 1 : primitive.maxY;
  auto &coef = primitive.getPlaneCoefficient();

  size_t i = polygons.size();
  polygons.emplace_back(i, coef);
  polygonTable[y].push_back(i);

  const auto &p = primitive.projected;
  const auto &t = primitive.transformed;
  const auto num = 3;
  for (int j = 0; j < num; j++) {
    auto &p1 = p[j];
    auto &p2 = p[(j + 1) % num];

    auto upperIdx = p1[1] > p2[1] ? j : (j + 1) % num;

    auto &upper = p1[1] > p2[1] ? p1 : p2;
    auto &lower = p1[1] > p2[1] ? p2 : p1;
    auto deltaX = upper[0] - lower[0];
    auto deltaY = upper[1] - lower[1];
    auto upperZ = t[upperIdx][2];

    if (upper[1] < 0) {
      continue;
    }

    ScanEdge edge {i, upper[0], upper[1],
                  deltaY <= 0 ? 0 : -static_cast<float>(deltaX) / deltaY, deltaY, upperZ};

    if (edge.dy >= 1) {
      auto y = (size_t)upper[1];
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

bool Scaner::scan(size_t y) {
  auto &edgeIndexes = edgeTable[y];
  auto &polygonIndexes = polygonTable[y];

  for (int x = 0; x < width; x++) {
    zbuffer[x] = FLT_MIN;
  }

  if (edgeIndexes.size() == 0) {
    return false;
  }

  vector<size_t> unfinished;

  int polygonCount = 0;
  float left = 0.f, right = 0.f;

  sort(edgeIndexes.begin(), edgeIndexes.end(),
       [&](size_t a, size_t b) { return edges[a].x < edges[b].x; });

  for (auto &index : edgeIndexes) {
    auto &edge = edges[index];
    auto &polygon = polygons[edge.polygonId];

    right = min(edge.x, (float)width - 1);
    if (left >= right) {
      if (polygon.isIn) {
        polygon.isIn = false;
        polygonCount--;
      } else {
        polygon.isIn = true;
        polygonCount++;
      }
      continue;
    }

    if (polygonCount == 1) {
      size_t pos = 0;
      for (auto &idx : polygonIndexes) {
        if (polygons[idx].isIn) {
          pos = idx;
          break;
        }
      }
      auto &polygon = polygons[pos];
      auto z =
          -(float)(polygon.A * left + polygon.B * y + polygon.D) / polygon.C;
      for (int x = (int)left; x <= (int)right; x++) {
        zbuffer[x] = z;
        z += polygon.dzx;
      }
    } else if (polygonCount > 1) {
      vector<size_t> actives;
      for (auto idx : polygonIndexes) {
        auto &polygon = polygons[idx];
        if (!polygon.isIn) {
          continue;
        }
        actives.push_back(idx);
      }

      float maxZL = -1000.f, maxZR = -1000.f;
      size_t maxIndex = 0;
      bool hasThrough = false;
      for (auto idx : actives) {
        auto &poly = polygons[idx];
        auto zl = -(float)(poly.A * left + poly.B * y + poly.D) / poly.C;
        auto zr = -(float)(poly.A * right + poly.B * y + poly.D) / poly.C;
        if ((zl - maxZL) * (zr - maxZR) < 0) {
          hasThrough = true;
          break;
        }
        if (zl >= maxZL || zr >= maxZR) {
          maxZL = max(zl, maxZL);
          maxZR = max(zr, maxZR);
          maxIndex = poly.id;
        }
      }

      if (hasThrough) {
        for (auto x = floor(left); x <= ceil(right); x++) {
          for (auto idx : polygonIndexes) {
            auto &polygon = polygons[idx];
            if (!polygon.isIn) {
              continue;
            }
            if (polygon.C == 0)
              continue;
            auto z =
                -(polygon.A * x + polygon.B * y + polygon.D) / polygon.C;
            zbuffer[x] = max(z, zbuffer[x]);
          }
        }
      } else {
        auto &polygon = polygons[maxIndex];
        auto z = maxZL;
        for (int x = (int)left; x <= (int)right; x++) {
          zbuffer[x] = z;
          z += polygon.dzx;
        }
      }
    }

    if (polygon.isIn) {
      polygon.isIn = false;
      polygonCount--;
    } else {
      polygon.isIn = true;
      polygonCount++;
    }

    left = right;
  }

  for (auto &index : edgeIndexes) {
    auto &edge = edges[index];
    auto &polygon = polygons[edge.polygonId];
    polygon.isIn = false;

    edge.dy--;
    edge.x += edge.k;
    edge.z += edge.k * polygon.dzx + polygon.dzy;
    if (edge.dy >= 1) {
      unfinished.push_back(index);
    }
  }

  if (!unfinished.empty() && y > 0) {
    auto &next = edgeTable[y - 1];
    next.insert(next.end(), unfinished.begin(), unfinished.end());
    auto &nextPolygons = polygonTable[y - 1];
    for (auto &idx : next) {
      auto &edge = edges[idx];
      nextPolygons.push_back(edge.polygonId);
    }
  }

  return true;
}
