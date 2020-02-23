#include "Context/SoftwareRasterizer/Context.hpp"
#include "Context/SoftwareRasterizer/Rasterizer.hpp"

using namespace std;
using namespace Eigen;

namespace RB {

SoftwareRasterizerContext::SoftwareRasterizerContext() {
  auto &uniforms = rasterizer.uniform;
  auto vertex_shader = [&uniforms](const Attributes &attributes,
                                   Varyings &varyings, Vector4f &position) {
    const auto a_position = get<0>(attributes);
    auto &a_normal = get<1>(attributes);
    auto &a_uv = get<2>(attributes);

    auto &v_position = get<0>(varyings);
    auto &v_normal = get<1>(varyings);
    auto &v_uv = get<2>(varyings);

    position = uniforms.matrix * uniforms.model *
               Vector4f(a_position[0], a_position[1], a_position[2], 1.0f);

    v_position = {position[0], position[1], position[2]};
    v_normal = {a_normal[0], a_normal[1], a_normal[2]};
    v_uv = {a_uv[0], a_uv[1]};
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
  rasterizer.set_vertex_shader(move(vertex_shader));
  rasterizer.set_fragment_shader(move(fragment_shader));

  rasterizer.set_frame(&frame);
}

void SoftwareRasterizerContext::add(const Model &model) {
  auto origin_vao_num = vaos.size();
  auto geometry_num = 0;
  for (auto &mesh : model.meshes) {
    geometry_num += mesh.geometries.size();
  }

  vaos.resize(origin_vao_num + geometry_num);
  model_matrixs.resize(origin_vao_num + geometry_num);
  counts.resize(origin_vao_num + geometry_num);
  materials.resize(origin_vao_num + geometry_num);

  auto idx = origin_vao_num;
  for (const auto &mesh : model.meshes) {
    auto &model_matrix = mesh.model_matrix;
    for (auto &geometry : mesh.geometries) {
      auto vao = rasterizer.gen_vertex_array();
      vaos[idx] = vao;
      rasterizer.bind_vertex_array(vao);

      auto &material = geometry.material;
      materials[idx] = material;
      model_matrixs[idx] = model_matrix;

      rasterizer.element_buffer_data(geometry.indices.data());
      counts[idx] = geometry.index_count;

      rasterizer.vertex_attributes(
          Attributes{reinterpret_cast<const float *>(geometry.buffers.data()),
                     reinterpret_cast<const float *>(geometry.buffers.data()),
                     reinterpret_cast<const float *>(geometry.buffers.data())});

      rasterizer.vertex_attributes_pointer(0, 3, 5, 0); // position
      rasterizer.vertex_attributes_pointer(1, 3, 5, 3); // normal
      rasterizer.vertex_attributes_pointer(2, 2, 6, 6); // uv

      idx += 1;
    }
  }
}

void SoftwareRasterizerContext::draw() {
  frame.clear();
  for (size_t i = 0; i < 6; i++) {
    rasterizer.bind_vertex_array(vaos[i]);
    rasterizer.uniform.model = model_matrixs[i];
    rasterizer.uniform.material = materials[i];
    rasterizer.drawElements(counts[i]);
  }
}

void SoftwareRasterizerContext::set_view(const Eigen::Matrix4f &view_matrix) {
  rasterizer.uniform.matrix = view_matrix;
}

void SoftwareRasterizerContext::view_port(uint32_t width, uint32_t height) {
  frame.resize(width, height);
  rasterizer.view_port(width, height);
}

auto SoftwareRasterizerContext::get_colors() -> const std::vector<float> & {
  return frame.getColors();
};

} // namespace RB