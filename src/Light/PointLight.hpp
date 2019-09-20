#pragma once
#include "Light/Light.hpp"
#include <Eigen/Core>
#include <utility>

struct PointLight : Light {
  Eigen::Vector3f position;
};
