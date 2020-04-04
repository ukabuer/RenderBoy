#pragma once
#include "Application.hpp"
#include <RenderBoy/Context.hpp>
#include <RenderBoy/ModelLoader.hpp>

namespace RB {

class GLTFViewerSoftware final : public Application {
public:
  GLTFViewerSoftware() : control(800, 600) {}

  void prepare() final;
  void render() final;
  void process_cmd(int argc, const char **argv) final;
  TrackballControl *get_controller() final { return &(this->control); };
  const char *get_title() final { return "GLTFViewerSoftware"; }
  std::array<int32_t, 2> get_size() final { return {800, 600}; }

private:
  Model model;
  std::unique_ptr<ModelLoader> loader;
  int texture = 0;
  Camera camera = {};
  TrackballControl control;
  std::string path;
  std::unique_ptr<Context> context;
};

} // namespace RB