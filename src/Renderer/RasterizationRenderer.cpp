#include "Renderer/RasterizationRenderer.hpp"
#include "Material/DepthMaterial.hpp"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <array>
#include <functional>

using namespace std;
using namespace Eigen;

Frame &RasterizationRenderer::render(const Model &model, const Camera &camera) {
  struct Uniforms {
    Matrix4f matrix;
    std::vector<Light> lights;
    DepthMaterial material;
  } uniforms = {};

  using Attributes = tuple<const float *, const float *, const float *>;
  using Varyings =
      tuple<array<float, 3>, array<float, 3>, array<float, 3>, float>;

  frame.clear(background);

  // no model transform now
  const Matrix4f viewMatrix = camera.getViewMatrix().inverse();
  const Matrix4f &projectionMatrix = camera.getCullingProjectionMatrix();
  uniforms.matrix = projectionMatrix * viewMatrix;

  auto vertex_shader = [&uniforms](const Attributes &attributes,
                                   Varyings &varyings, Vector4f &position) {
    const auto a_position = get<0>(attributes);
    auto &a_normal = get<1>(attributes);
    auto &a_uv = get<2>(attributes);

    auto &v_position = get<0>(varyings);
    auto &v_normal = get<1>(varyings);
    auto &v_uv = get<2>(varyings);
    auto &v_depth = get<3>(varyings);

    position = uniforms.matrix *
               Vector4f(a_position[0], a_position[1], a_position[2], 1.0f);

    v_position = {position[0], position[1], position[2]};
    v_normal = {a_normal[0], a_normal[1], a_normal[2]};
    v_uv = {a_uv[0], a_uv[1]};
    v_depth = position[2] / position[3];
  };

  auto fragment_shader = [&uniforms](const Varyings &varyings,
                                     Vector4f &color) {
    auto &v_depth = get<3>(varyings);

    color = uniforms.material.sample(v_depth);
    //    color = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
  };

  Rasterizer<Uniforms, Attributes, Varyings> rasterizer;
  rasterizer.set_frame(&frame);
  rasterizer.set_vertex_shader(move(vertex_shader));
  rasterizer.set_fragment_shader(move(fragment_shader));

  for (const auto &mesh : model.meshes) {
    for (const auto &geometry : mesh.geometries) {
      rasterizer.vertex_attributes(
          Attributes{reinterpret_cast<const float *>(geometry.buffers.data()),
                     reinterpret_cast<const float *>(geometry.buffers.data()),
                     reinterpret_cast<const float *>(geometry.buffers.data())});

      rasterizer.vertex_attributes_pointer(0, 3, 5, 0); // position
      rasterizer.vertex_attributes_pointer(1, 3, 5, 3); // normal
      rasterizer.vertex_attributes_pointer(2, 2, 6, 4); // uv

      if (geometry.indices.empty()) {
        rasterizer.drawArray(geometry.vertex_count);
      } else {
        rasterizer.drawElements(geometry.indices, geometry.index_count);
      }
    }
  }

  return frame;
}
