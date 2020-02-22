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
  frame.clear(background);

  return frame;
}

} // namespace RB