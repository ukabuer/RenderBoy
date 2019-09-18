#pragma once

#include "Eigen/Core"

struct Light {
  float intensity;
  Eigen::Vector3f color = {1.0f, 1.0f, 1.0f};
};
