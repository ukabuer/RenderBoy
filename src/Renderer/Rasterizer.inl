#include "Rasterizer.hpp"
#include "utils.hpp"
#include <tuple>
#include <utility>

template <typename T> struct is_std_array : std::false_type {};

template <typename T, size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template <typename Array,
          typename enable = std::enable_if_t<is_std_array<Array>::value>>
Array operator+(const Array &a, const Array &b) {
  Array c;
  constexpr auto size = c.size();
  for (size_t idx = 0; idx < size; idx++) {
    c[idx] = a[idx] + b[idx];
  }
  return c;
}

template <typename Array,
          typename enable = std::enable_if_t<is_std_array<Array>::value>>
Array operator*(const Array &a, float b) {
  Array c;
  constexpr auto size = c.size();
  for (size_t idx = 0; idx < size; idx++) {
    c[idx] = a[idx] * b;
  }
  return c;
}

template <typename Array,
          typename enable = std::enable_if_t<is_std_array<Array>::value>>
Array operator/(const Array &a, float b) {
  Array c;
  constexpr auto size = c.size();
  for (size_t idx = 0; idx < size; idx++) {
    c[idx] = a[idx] / b;
  }
  return c;
}

template <typename Tuple, size_t... I>
auto apply(const std::array<float, 3> &coef, Tuple &&t1, Tuple &&t2, Tuple &&t3,
           std::index_sequence<I...>) {
  return std::make_tuple(std::get<I>(std::forward<Tuple>(t1)) * coef[0] +
                         std::get<I>(std::forward<Tuple>(t2)) * coef[1] +
                         std::get<I>(std::forward<Tuple>(t3)) * coef[2]...);
}

template <typename Tuple>
auto interpolate(const std::array<float, 3> &coef, Tuple &&t1, Tuple &&t2,
                 Tuple &&t3) {
  constexpr size_t size = std::tuple_size<std::decay_t<Tuple>>::value;
  return apply(coef, std::forward<Tuple>(t1), std::forward<Tuple>(t2),
               std::forward<Tuple>(t3), std::make_index_sequence<size>());
}

template <typename Attributes, size_t... I>
auto extract_attribute_helper(Attributes &attribute,
                              const Attributes &all_attributes,
                              const std::vector<AttributeObject> &usages,
                              uint32_t count, std::index_sequence<I...>) {
  using expand = bool[];
  (void)expand{(std::get<I>(attribute) =
                    std::get<I>(all_attributes) + usages[I].offset +
                    count * (usages[I].components + usages[I].stride),
                true)...};
}

template <typename Attributes>
auto extract_attribute(Attributes &attribute, const Attributes &all_attributes,
                       const std::vector<AttributeObject> &usages,
                       uint32_t count) {
  constexpr size_t size = std::tuple_size<std::decay_t<Attributes>>::value;
  extract_attribute_helper(attribute, all_attributes, usages, count,
                           std::make_index_sequence<size>());
}

inline auto orient2d(const std::array<int, 2> &a, const std::array<int, 2> &b,
                     const std::array<int, 2> &c) -> int {
  return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]);
}

inline auto max3(int a, int b, int c) -> int {
  auto tmp = a > b ? a : b;
  return c > tmp ? c : tmp;
}

inline auto min3(int a, int b, int c) -> int {
  auto tmp = a < b ? a : b;
  return c < tmp ? c : tmp;
}

template <typename Uniforms, typename Attributes, typename Varyings>
void Rasterizer<Uniforms, Attributes, Varyings>::drawArray(uint32_t count) {
  const uint8_t components = 3; // only support triangle now

  homo.resize(count);
  depth.resize(count);
  screen_coords.resize(count);
  all_varyings.resize(count);

  for (size_t i = 0; i < count; i++) {
    Attributes attributes;
    extract_attribute(attributes, all_attributes, attributes_pointers, i);

    Eigen::Vector4f value{};
    vertex_shader(attributes, all_varyings[i], value);

    depth[i] = value[2] / value[3];
    homo[i] = value[3];
    screen_coords[i] = {
        static_cast<int>((value[0] / value[3] + 1.f) * screen[0] / 2.0f),
        static_cast<int>((value[1] / value[3] + 1.f) * screen[1] / 2.0f)};
  }

  const uint32_t triangle_num = count / components;
  ParallelForEach(static_cast<uint32_t>(0), triangle_num, [this](auto i) {
    uint32_t start = i * 3;
    this->traverse_triangle(start, start + 1, start + 2);
  });
}

template <typename Uniforms, typename Attributes, typename Varyings>
void Rasterizer<Uniforms, Attributes, Varyings>::traverse_triangle(
    uint32_t v0_index, uint32_t v1_index, uint32_t v2_index) {
  const auto width = static_cast<int>(screen[0]);
  const auto height = static_cast<int>(screen[1]);

  const auto v0_screen_coords = screen_coords[v0_index];
  const auto v1_screen_coords = screen_coords[v1_index];
  const auto v2_screen_coords = screen_coords[v2_index];
  // compute bounding box and clip against screen bounds
  auto minX = fmaxf(
      min3(v0_screen_coords[0], v1_screen_coords[0], v2_screen_coords[0]), 0);
  auto minY = fmaxf(
      min3(v0_screen_coords[1], v1_screen_coords[1], v2_screen_coords[1]), 0);
  auto maxX =
      fmin(max3(v0_screen_coords[0], v1_screen_coords[0], v2_screen_coords[0]),
           width - 1);
  auto maxY =
      fminf(max3(v0_screen_coords[1], v1_screen_coords[1], v2_screen_coords[1]),
            height - 1);

  // increment for weight on rows and columns
  const Eigen::Vector3i A = {v1_screen_coords[1] - v2_screen_coords[1],
                             v2_screen_coords[1] - v0_screen_coords[1],
                             v0_screen_coords[1] - v1_screen_coords[1]};
  const Eigen::Vector3i B = {v2_screen_coords[0] - v1_screen_coords[0],
                             v0_screen_coords[0] - v2_screen_coords[0],
                             v1_screen_coords[0] - v0_screen_coords[0]};

  std::array<int, 2> screen_coord = {static_cast<int>(minX),
                                     static_cast<int>(minY)};

  Eigen::Vector3i rowWeight = {
      orient2d(v1_screen_coords, v2_screen_coords, screen_coord),
      orient2d(v2_screen_coords, v0_screen_coords, screen_coord),
      orient2d(v0_screen_coords, v1_screen_coords, screen_coord)};

  const auto v0_homo = homo[v0_index];
  const auto v1_homo = homo[v1_index];
  const auto v2_homo = homo[v2_index];

  const auto v0_depth = depth[v0_index];
  const auto v1_depth = depth[v1_index];
  const auto v2_depth = depth[v2_index];

  for (screen_coord[1] = minY; screen_coord[1] <= maxY;
       screen_coord[1]++, rowWeight += B) {
    Eigen::Vector3i weight = rowWeight;

    const auto sum = weight.sum();
    for (screen_coord[0] = minX; screen_coord[0] <= maxX;
         screen_coord[0]++, weight += A) {
      if (weight[0] < 0 || weight[1] < 0 || weight[2] < 0 || sum == 0) {
        continue;
      }

      std::array<float, 3> clip = {
          static_cast<float>(weight[0]) / (v0_homo * sum),
          static_cast<float>(weight[1]) / (v1_homo * sum),
          static_cast<float>(weight[2]) / (v2_homo * sum)};
      clip = clip / (clip[0] + clip[1] + clip[2]);

      auto current_depth =
          -(clip[0] * v0_depth + clip[1] * v1_depth + clip[2] * v2_depth);

      // depth test
      if (current_depth < -1.f || current_depth > 1.f) {
        continue;
      }

      const auto idx =
          static_cast<size_t>(screen_coord[0] + screen_coord[1] * width);

      if (current_depth <= frame->getZ(idx)) {
        continue;
      }
      frame->setZ(idx, current_depth);

      const auto &v1_varying = all_varyings[v0_index];
      const auto &v2_varying = all_varyings[v1_index];
      const auto &v3_varying = all_varyings[v2_index];

      // for each varying, do interpolating, maybe we can use SIMD
      auto varyings = interpolate(clip, v1_varying, v2_varying, v3_varying);

      Eigen::Vector4f color = {0.0f, 0.0f, 0.0f, 0.0f};
      fragment_shader(varyings, color);
      frame->setColor(idx, color);
    }
  }
}

template <typename Uniforms, typename Attributes, typename Varyings>
void Rasterizer<Uniforms, Attributes, Varyings>::drawElements(
    const std::vector<uint32_t> &indices, uint32_t index_count) {
  const uint8_t components = 3; // only support triangle now

  if (frame == nullptr)
    return;

  homo.resize(index_count);
  depth.resize(index_count);
  screen_coords.resize(index_count);
  all_varyings.resize(index_count);

  for (size_t i = 0; i < index_count; i++) {
    auto vertex_idx = indices[i];
    Attributes attributes;
    extract_attribute(attributes, all_attributes, attributes_pointers,
                      vertex_idx);

    Eigen::Vector4f value{};
    vertex_shader(attributes, all_varyings[i], value);

    depth[vertex_idx] = value[2] / value[3];
    homo[vertex_idx] = value[3];
    screen_coords[vertex_idx] = {
        static_cast<int>((value[0] / value[3] + 1.f) * screen[0] / 2.0f),
        static_cast<int>((value[1] / value[3] + 1.f) * screen[1] / 2.0f)};
  }

  const uint32_t triangle_num = index_count / components;
  ParallelForEach(static_cast<uint32_t>(0), triangle_num,
                  [this, indices](auto i) {
                    uint32_t start = i * 3;
                    auto v0_index = indices[start];
                    auto v1_index = indices[start + 1];
                    auto v2_index = indices[start + 2];

                    this->traverse_triangle(v0_index, v1_index, v2_index);
                  });
}
