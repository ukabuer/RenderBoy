#include "RasterizationRenderer.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <RenderBoy/Material.hpp>
#include <array>
#include <functional>

using namespace std;
using namespace Eigen;

namespace RB {

const Frame &RasterizationRenderer::render(const Model &model,
                                           const Camera &camera) {
  struct Uniforms {
    Matrix4f matrix;
    Matrix4f model;
    Material material;
    Texture shadow_map;
  } uniforms = {};
  using Attributes = tuple<const float *, const float *, const float *>;
  using Varyings =
      tuple<array<float, 3>, array<float, 3>, array<float, 2>, float>;

  Rasterizer<Uniforms, Attributes, Varyings> rasterizer;
  rasterizer.set_frame(&frame);

  frame.clear(background);

  // render pass
  auto vertex_shader = [&uniforms](const Attributes &attributes,
                                   Varyings &varyings, Vector4f &position) {
    const auto a_position = get<0>(attributes);
    auto &a_normal = get<1>(attributes);
    auto &a_uv = get<2>(attributes);

    auto &v_position = get<0>(varyings);
    auto &v_normal = get<1>(varyings);
    auto &v_uv = get<2>(varyings);
    auto &v_depth = get<3>(varyings);

    position = uniforms.matrix * uniforms.model *
               Vector4f(a_position[0], a_position[1], a_position[2], 1.0f);

    v_position = {position[0], position[1], position[2]};
    v_normal = {a_normal[0], a_normal[1], a_normal[2]};
    v_uv = {a_uv[0], a_uv[1]};
    v_depth = position[2] / position[3];
  };

  auto fragment_shader = [&uniforms](const Varyings &varyings,
                                     Vector4f &color) {
    auto &v_normal = get<1>(varyings);
    auto &v_uv = get<2>(varyings);

    auto &material = uniforms.material;
    if (material.base_color_texture != nullptr) {
      color = material.base_color_texture->sample(v_uv[0], v_uv[1]);
    } else {
      color = Vector4f(material.base_color[0], material.base_color[1],
                       material.base_color[2], material.base_color[3]);
    }
  };

  frame.clear(background);
  rasterizer.set_vertex_shader(move(vertex_shader));
  rasterizer.set_fragment_shader(move(fragment_shader));

  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f &projectionMatrix = camera.getCullingProjectionMatrix();
  uniforms.matrix = projectionMatrix * viewMatrix;

  for (const auto &mesh : model.meshes) {
    uniforms.model = mesh.model_matrix;
    for (const auto &geometry : mesh.geometries) {
      uniforms.material = geometry.material;
      rasterizer.vertex_attributes(
          Attributes{reinterpret_cast<const float *>(geometry.buffers.data()),
                     reinterpret_cast<const float *>(geometry.buffers.data()),
                     reinterpret_cast<const float *>(geometry.buffers.data())});

      rasterizer.vertex_attributes_pointer(0, 3, 5, 0); // position
      rasterizer.vertex_attributes_pointer(1, 3, 5, 3); // normal
      rasterizer.vertex_attributes_pointer(2, 2, 6, 6); // uv
      if (geometry.indices.empty()) {
        rasterizer.drawArray(geometry.vertex_count);
      } else {
        rasterizer.drawElements(geometry.indices, geometry.index_count);
      }
    }
  }

  return frame;
}

} // namespace RB