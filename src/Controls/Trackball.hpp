#pragma once
#include "Camera.hpp"
#include "Geometry.hpp"

class TrackballControl {
public:
  explicit TrackballControl(Camera &camera) : camera(camera) {}

  void onMouseDown(bool isLeft, int x, int y) {
    if (isLeft) {
      this->isOrbit = true;
    } else {
      this->isPan = true;
    }
    this->lastX = x;
    this->lastY = y;
  }

  void onMouseUp(bool isLeft, int x, int y) {
    if (isLeft) {
      this->isOrbit = false;
    } else {
      this->isPan = false;
    }
    this->lastX = x;
    this->lastY = y;
  }

  void onMouseWheelScroll(float delta) {
    this->zoomDelta = delta;
    this->isZoom = true;
    this->update(lastX, lastY);
    this->isZoom = false;
  }

  void onMouseMove(int x, int y) {
    if (isOrbit || isPan || isZoom) {
      update(x, y);
    }
  }

private:
  Camera &camera;

  bool isOrbit = false;
  bool isPan = false;
  bool isZoom = false;

  int lastX = 0;
  int lastY = 0;

  float zoomDelta = 0.f;

  void update(int x, int y);
};
