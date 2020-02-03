#pragma once
#include <Eigen/Core>
#include <RenderBoy/Frame.hpp>
#include <array>
#include <functional>
#include <utility>
#include <vector>

namespace RB {

struct AttributeObject {
  uint8_t components = 1;
  uint32_t stride = 0;
  uint32_t offset = 0;
};

template <typename Uniforms, typename Attributes, typename Varyings>
class Rasterizer {
  using VertexShader =
      std::function<void(const Attributes &, Varyings &, Eigen::Vector4f &)>;
  using FragmentShader =
      std::function<void(const Varyings &, Eigen::Vector4f &)>;

public:
  Rasterizer() = default;

  void drawArray(uint32_t count);

  void drawElements(const std::vector<uint32_t> &indices, uint32_t count);

  void traverse_triangle(uint32_t v0_index, uint32_t v1_index,
                         uint32_t v2_index);

  void vertex_attributes_pointer(uint32_t location, uint8_t components,
                                 uint32_t stride, uint32_t offset) {
    this->attributes_pointers[location] = {components, stride, offset};
  }

  void vertex_attributes(Attributes attributes) {
    this->all_attributes = move(attributes);
    auto size = std::tuple_size<std::decay_t<Attributes>>::value;
    this->attributes_pointers.resize(size);
  }

  void set_vertex_shader(VertexShader shader) {
    this->vertex_shader = move(shader);
  }

  void set_fragment_shader(FragmentShader shader) {
    this->fragment_shader = move(shader);
  }

  void set_frame(Frame *_frame) {
    this->frame = _frame;
    this->screen[0] = _frame->getWidth();
    this->screen[1] = _frame->getHeight();
  }

private:
  Frame *frame = nullptr;
  Uniforms uniform;
  std::vector<AttributeObject> attributes_pointers;
  Attributes all_attributes;
  std::vector<Varyings> all_varyings;
  VertexShader vertex_shader;
  FragmentShader fragment_shader;
  std::array<uint32_t, 2> screen = {0, 0};
  std::vector<float> homo;
  std::vector<float> depth;
  std::vector<std::array<int, 2>> screen_coords;
};

} // namespace RB

#include "Rasterizer.inl"