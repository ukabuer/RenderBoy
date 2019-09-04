#pragma once

#include "Renderer/Renderer.hpp"

class ScanLineRenderer final: public Renderer {
public:
  ~ScanLineRenderer() {}

  virtual Frame render(const Scene &scene, const Camera &camera) override;
};
