#pragma once
#include "Camera/OrthographicCamera.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Geometry.hpp"

class OrbitController {
public:
  OrbitController(PerspectiveCamera &camera)
      : camera(camera), isOrbit(false), orbitPos{0, 0}, orbitDelta{0.f, 0.f},
        isPan(false), panPos{0, 0}, panDelta{0.f, 0.f}, isZoom(false),
        zoomDelta(0.f) {}

  void updateOrbit(bool isOrbit, int x, int y) {
    this->isOrbit = isOrbit;
    this->orbitPos[0] = x;
    this->orbitPos[1] = y;
  }

  void updatePan(bool isPan, int x, int y) {
    this->isPan = isPan;
    this->panPos[0] = x;
    this->panPos[1] = y;
  }

  void updateZoom(float delta, int x, int y) {
    this->zoomDelta = delta;
    this->isZoom = true;
    this->update(x, y);
    this->isZoom = false;
  }

  void update(int x, int y);

private:
  PerspectiveCamera &camera;

  bool isOrbit;
  int orbitPos[2];
  float orbitDelta[2];

  bool isPan;
  int panPos[2];
  float panDelta[2];

  bool isZoom;
  float zoomDelta;
};
