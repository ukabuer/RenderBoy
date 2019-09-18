#pragma once

#include "Camera/PerspectiveCamera.hpp"

constexpr inline float clamp(float f, float min, float max) noexcept {
  return f < min ? min : (f > max ? max : f);
}

inline Eigen::Vector3f calculatePan(const PerspectiveCamera &camera,
                                    const Eigen::Vector3f &translate,
                                    const float panDelta[]) {
  const Eigen::Vector3f forward = translate.normalized();
  const Eigen::Vector3f left = camera.getUp().cross(forward);
  const Eigen::Vector3f up = forward.cross(left);

  const auto fov = camera.getFOV() * PI / 180.f;
  const auto distance = translate.norm();
  const auto factor = distance * tan(fov / 2) * 2;
  const Eigen::Vector3f delta_x = (panDelta[0] * factor) * left;
  const Eigen::Vector3f delta_y = (panDelta[1] * factor) * up;
  return delta_x + delta_y;
}

inline Eigen::Vector3f calculateMove(const PerspectiveCamera &camera,
                                     const Eigen::Vector3f &translate,
                                     const float orbitDelta[],
                                     float zoomDelta) {
  float radius = translate.norm();
  auto theta = atan2(translate[0], translate[2]);             /* azimuth */
  auto phi = acos(translate[1] / radius); /* polar */
  const auto factor = PI * 0.1f;
  const auto EPSILON = 1e-5f;
  Eigen::Vector3f offset;

  radius *= static_cast<float>(pow(0.95, zoomDelta));
  theta -= orbitDelta[0] * factor;
  phi -= orbitDelta[1] * factor;
  phi = clamp(phi, EPSILON, PI - EPSILON);

  offset[0] = radius * sin(phi) * sin(theta);
  offset[1] = radius * cos(phi);
  offset[2] = radius * sin(phi) * cos(theta);

  return offset;
}

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

  void update(int x, int y) {
    if (!isOrbit && !isPan && !isZoom) {
      return;
    }

    const auto height = camera.getHeight();
    if (isOrbit) {
      orbitDelta[0] = static_cast<float>(orbitPos[0] - x) / height;
      orbitDelta[1] = static_cast<float>(orbitPos[1] - y) / height;
    }

    if (isPan) {
      panDelta[0] = static_cast<float>(panPos[0] - x) / height;
      panDelta[1] = static_cast<float>(panPos[1] - y) / height;
    }

    const Eigen::Vector3f fromTarget =
        camera.getPosition() - camera.getTarget();
    const Eigen::Vector3f fromCamera = -fromTarget;

    const auto pan = calculatePan(camera, fromCamera, panDelta);
    const auto offset =
        calculateMove(camera, fromTarget, orbitDelta, zoomDelta);

    camera.setTarget(camera.getTarget() + pan);
    camera.setPosition(camera.getTarget() + offset);

    orbitDelta[0] = orbitDelta[1] = 0.f;
    panDelta[0] = panDelta[1] = 0.f;
    zoomDelta = 0.f;
  }

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
