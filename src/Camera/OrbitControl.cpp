#include "Camera/OrbitControl.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;

constexpr auto clamp(float f, float min, float max) noexcept -> float {
  return f < min ? min : (f > max ? max : f);
}

inline auto calculatePan(const PerspectiveCamera &camera,
                         const Vector3f &translate,
                         const float panDelta[]) -> Vector3f {
  const Vector3f forward = translate.normalized();
  const Vector3f left = camera.getUp().cross(forward);
  const Vector3f up = forward.cross(left);

  const auto fov = camera.getFOV() * PI / 180.f;
  const auto distance = translate.norm();
  const auto factor = distance * tan(fov / 2) * 2;
  const Vector3f delta_x = (panDelta[0] * factor) * left;
  const Vector3f delta_y = (panDelta[1] * factor) * up;
  return delta_x + delta_y;
}

inline auto calculateMove(const Vector3f &translate,
                          const float orbitDelta[], float zoomDelta)
    -> Vector3f {
  float radius = translate.norm();
  auto theta = atan2(translate[0], translate[2]); /* azimuth */
  auto phi = acos(translate[1] / radius);         /* polar */
  const auto factor = PI * 0.1f;
  const auto EPSILON = 1e-5f;
  Vector3f offset;

  radius *= static_cast<float>(pow(0.95, zoomDelta));
  theta -= orbitDelta[0] * factor;
  phi -= orbitDelta[1] * factor;
  phi = clamp(phi, EPSILON, PI - EPSILON);

  offset[0] = radius * sin(phi) * sin(theta);
  offset[1] = radius * cos(phi);
  offset[2] = radius * sin(phi) * cos(theta);

  return offset;
}

void OrbitController::update(int x, int y) {
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

  const Vector3f fromTarget = camera.getPosition() - camera.getTarget();
  const Vector3f fromCamera = -fromTarget;

  const auto pan = calculatePan(camera, fromCamera, panDelta);
  const auto offset = calculateMove(fromTarget, orbitDelta, zoomDelta);

  camera.setTarget(camera.getTarget() + pan);
  camera.setPosition(camera.getTarget() + offset);

  orbitDelta[0] = orbitDelta[1] = 0.f;
  panDelta[0] = panDelta[1] = 0.f;
  zoomDelta = 0.f;
}