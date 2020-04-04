#pragma once
#include <RenderBoy/Controls/Trackball.hpp>
#include <array>
#include <cstdint>

namespace RB {

class Application {
public:
  virtual ~Application() = default;
  virtual void process_cmd(int argc, const char **argv){};
  virtual void prepare() = 0;
  virtual void render() = 0;
  virtual std::array<int32_t, 2> get_size() { return {800, 600}; }
  virtual const char *get_title() { return "RenderBoy"; }
  virtual TrackballControl *get_controller() { return nullptr; }
};

extern Application *create_application();

} // namespace RB
