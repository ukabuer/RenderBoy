#pragma once
#include <Eigen/Core>
#include <RenderBoy/Frame.hpp>
#include <RenderBoy/Model.hpp>

namespace RB {

class IContextImpl {
public:
  virtual void add(const Model &model) = 0;
  virtual void draw() = 0;
  virtual void set_view(const Eigen::Matrix4f &view_matrix) = 0;
  virtual void view_port(uint32_t width, uint32_t height) = 0;
  virtual auto get_colors() -> const std::vector<float> & = 0;
};

} // namespace RB