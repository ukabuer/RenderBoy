#include "RenderBoy/Controls/Trackball.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace std;
using namespace Eigen;

static auto clamp(float f, float min, float max) noexcept -> float {
  return f < min ? min : (f > max ? max : f);
}

static auto calculateZoom(const Vector3f &from, const Vector3f &to, float zoomDelta) {
  const auto factor = 0.1f;
  const Vector3f direction = to - from;
  return zoomDelta * factor * direction;
}

static auto calculateRotate(const Vector3f &from,
                          const Vector3f &to,
                          const int deltaX,
                          const int deltaY)
    -> Vector3f {
  const auto factor = 0.02f;
  const auto EPSILON = 1e-5f;

  const Vector3f direction = to - from;
  float radius = direction.norm();
  auto theta = atan2(direction[0], direction[2]); /* azimuth */
  auto phi = acos(direction[1] / radius);         /* polar */

  theta -= static_cast<float>(deltaX) * factor;
  phi -= static_cast<float>(deltaY) * factor;
  phi = clamp(phi, EPSILON, PI - EPSILON);

  return {radius * sin(phi) * sin(theta), radius * cos(phi), radius * sin(phi) * cos(theta)};
}

static auto calculatePan(const Vector3f &from,
                         const Vector3f &to,
                         const Vector3f& up,
                         const int deltaX,
                         const int deltaY) -> Vector3f {
  const Vector3f forward = (to - from).normalized();
  const Vector3f left = up.cross(forward);

  const auto distance = forward.norm();
  const auto factor = distance * 0.01f;

  const Vector3f offsetX = (static_cast<float>(deltaX) * factor) * left;
  const Vector3f offsetY = (static_cast<float>(deltaY) * factor) * up;
  return offsetX + offsetY;
}

void TrackballControl::update(int x, int y) {
  auto deltaX = x - lastX;
  auto deltaY = y - lastY;

  //  if (isZoom) {
  //    const auto offset = calculateZoom(camera.getPosition(),
  //    camera.getTarget(), zoomDelta); camera.setPosition(camera.getPosition()
  //    + offset);
  //  }
  //
  //  if (isOrbit) {
  //    const auto offset = calculateRotate(camera.getTarget(),
  //    camera.getPosition(), deltaX, deltaY);
  //    camera.setPosition(camera.getTarget() + offset);
  //  }
  //
  //  if (isPan) {
  //    const auto pan = calculatePan(camera.getPosition(), camera.getTarget(),
  //    camera.getUp(), deltaX, deltaY); camera.setTarget(camera.getTarget() +
  //    pan);
  //  }

  lastX = x;
  lastY = y;
  zoomDelta = 0.f;
}