#include "ScanLine.hpp"

using namespace std;

vector<size_t> scan(vector<size_t> &edgeIndexes, vector<size_t> &polygonIndexes,
                    vector<Edge> &edges, vector<Polygon> &polygons,
                    vector<float> &zbuffer, int y, int width) {
  vector<size_t> unfinished;

  int polygonCount = 0;
  float left = 0.f, right = 0.f;

  sort(edgeIndexes.begin(), edgeIndexes.end(),
       [&edges](size_t a, size_t b) { return edges[a].x < edges[b].x; });

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
          // 两端差值异号，说明贯穿
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
        for (int x = (int)floor(left); x <= (int)ceil(right); x++) {
          for (auto idx : polygonIndexes) {
            auto &polygon = polygons[idx];
            if (!polygon.isIn) {
              continue;
            }
            if (polygon.C == 0)
              continue;
            auto z =
                -(float)(polygon.A * x + polygon.B * y + polygon.D) / polygon.C;
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

  return unfinished;
}
