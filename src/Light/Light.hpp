#pragma once

#include <array>
#include <utility>

class Light {
public:
  Light(unsigned char r, unsigned char g, unsigned char b, unsigned char a): color({r, g, b, a}), intensity(1) {}

  explicit Light(std::array<unsigned char, 4> color) : color(std::move(color)), intensity(1) {}

  void setColor(unsigned char r, unsigned char g, unsigned char b,
                unsigned char a);
  const std::array<unsigned char, 4> &getColor() const { return this->color; }

  void setIntensity(float intensity) { this->intensity = intensity; }
  float getIntensity() const { return this->intensity; }

private:
  std::array<unsigned char, 4> color;
  float intensity;
};
