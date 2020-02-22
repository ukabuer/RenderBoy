#pragma once
#include <Eigen/Core>
#include <RenderBoy/Frame.hpp>
#include <RenderBoy/Model.hpp>
#include <memory>

namespace RB {

class IContextImpl;

class Context {
public:
  enum class Type : uint8_t { SoftwareRasterizer, OpenGL };

  explicit Context(Type type);
  ~Context();

  void add(const Model &model);
  void draw();
  void set_view(const Eigen::Matrix4f &view_matrix);
  void view_port(uint32_t width, uint32_t height);
  auto get_colors() -> const std::vector<float> &;

private:
  std::unique_ptr<IContextImpl> impl;
};

} // namespace RB