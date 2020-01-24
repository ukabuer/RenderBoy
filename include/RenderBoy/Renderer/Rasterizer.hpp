#pragma once
#include "Eigen/Core"
#include <array>
#include <functional>
#include <utility>
#include <vector>

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

  void set_frame(Frame *frame) {
    this->frame = frame;
    this->screen[0] = frame->getWidth();
    this->screen[1] = frame->getHeight();
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