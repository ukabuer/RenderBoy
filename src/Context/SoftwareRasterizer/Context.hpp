#include "Context/IContextImp.hpp"
#include "Context/SoftwareRasterizer/Rasterizer.hpp"

namespace RB {

class SoftwareRasterizerContext : public IContextImpl {
public:
  SoftwareRasterizerContext();

  void add(const Model &model) override;
  void draw() override;
  void set_view(const Eigen::Matrix4f &view_matrix) override;
  void view_port(uint32_t width, uint32_t height) override;
  auto get_colors() -> const std::vector<float> & override;

private:
  struct Uniforms {
    Eigen::Matrix4f matrix;
    Eigen::Matrix4f model;
    Material material;
  };
  using Attributes = std::tuple<const float *, const float *, const float *>;
  using Varyings = std::tuple<std::array<float, 3>, std::array<float, 3>,
                              std::array<float, 2>>;

  Rasterizer<Uniforms, Attributes, Varyings> rasterizer;
  std::vector<uint32_t> counts;
  std::vector<Eigen::Matrix4f> model_matrixs;
  std::vector<Material> materials;
  std::vector<uint32_t> vaos;
  Frame frame;
};

} // namespace RB