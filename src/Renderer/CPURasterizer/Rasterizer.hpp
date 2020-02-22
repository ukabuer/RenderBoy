#pragma once
#include <Eigen/Core>
#include <RenderBoy/Frame.hpp>
#include <array>
#include <cassert>
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
  struct VertexAttributeArray {
    std::vector<AttributeObject> attributes_pointers;
    Attributes attributes;
    const uint32_t *indices = nullptr;
  };
  using VertexShader =
      std::function<void(const Attributes &, Varyings &, Eigen::Vector4f &)>;
  using FragmentShader =
      std::function<void(const Varyings &, Eigen::Vector4f &)>;

public:
  Rasterizer() = default;

  void drawArray(uint32_t count);

  void drawElements(uint32_t count);

  void traverse_triangle(uint32_t v0_index, uint32_t v1_index,
                         uint32_t v2_index);

  uint32_t gen_vertex_array() {
    vertex_attribute_arrays.emplace_back();
    auto &array = vertex_attribute_arrays.back();
    array.attributes_pointers.resize(std::tuple_size<Attributes>());
    return vertex_attribute_arrays.size() - 1;
  }

  void bind_vertex_array(uint32_t idx) {
    assert(idx < vertex_attribute_arrays.size());
    current_vao = idx;
  }

  void element_buffer_data(const uint32_t *data) {
    auto &target = vertex_attribute_arrays[current_vao];
    target.indices = data;
  }

  void vertex_attributes_pointer(uint32_t location, uint8_t components,
                                 uint32_t stride, uint32_t offset) {
    auto &target = vertex_attribute_arrays[current_vao];
    target.attributes_pointers[location] = {components, stride, offset};
  }

  void vertex_attributes(Attributes attributes) {
    auto &vao = vertex_attribute_arrays[current_vao];
    vao.attributes = move(attributes);
    auto size = std::tuple_size<std::decay_t<Attributes>>::value;
    vao.attributes_pointers.resize(size);
  }

  void set_vertex_shader(VertexShader shader) {
    this->vertex_shader = move(shader);
  }

  void set_fragment_shader(FragmentShader shader) {
    this->fragment_shader = move(shader);
  }

  void set_frame(Frame *_frame) { this->frame = _frame; }

  void view_port(uint32_t width, uint32_t height) {
    this->screen[0] = width;
    this->screen[1] = height;
  }

  Uniforms uniform;

private:
  Frame *frame = nullptr;
  uint32_t current_vao = 0;
  std::vector<Rasterizer::VertexAttributeArray> vertex_attribute_arrays;
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