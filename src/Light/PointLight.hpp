#pragma once

#include "Light/Light.hpp"
#include <Eigen/Core>
#include <utility>

struct PointLight : Light {
  Eigen::Vector3f position;

  PointLight() = default;

  PointLight(Eigen::Vector3f color,
             Eigen::Vector3f position)
      : position(std::move(position)) {
    this->color = std::move(color);
  }
};
