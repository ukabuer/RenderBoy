#pragma once

#include "Light/Light.hpp"
#include <Eigen/Core>
#include <utility>

class PointLight : public Light {
public:
  PointLight(const std::array<unsigned char, 4> &color,
             Eigen::Vector3f position)
      : Light(color), position(std::move(position)) {}

  Eigen::Vector3f getPosition() const {
    return this->position;
  }

  void setPosition(float x, float y, float z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
  }

private:
  Eigen::Vector3f position;
};
