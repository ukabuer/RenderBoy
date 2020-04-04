#pragma once
#include "Application.hpp"
#include <RenderBoy/Context.hpp>

namespace RB {

class GLTFViewerOpenGL final : public Application {
public:
  GLTFViewerOpenGL() : control(800, 600) {}

  void prepare() final;
  void render() final;
  void process_cmd(int argc, const char **argv) final;
  TrackballControl *get_controller() final { return &(this->control); };
  const char *get_title() final { return "GLTFViewerOpenGL"; }
  std::array<int32_t, 2> get_size() final { return {800, 600}; }

private:
  Geometry geometry;
  Camera camera = {};
  TrackballControl control;
  std::string path;
  std::unique_ptr<Context> context;
};

} // namespace RB