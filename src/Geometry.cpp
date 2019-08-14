#include "Geometry.hpp"

using namespace std;
using namespace Eigen;

Geometry::Geometry(vector<Vector3f> v, vector<uint32_t> i) {
  this->vertices = move(v);
  this->indices = move(i);
}

unique_ptr<Geometry> Geometry::Box(float w, float h, float d) {
  using vec3 = Vector3f;
  auto hw = w / 2, hh = h / 2, hd = d / 2;
  vector<vec3> vertices = {
      vec3(hw, hh, -hd),   vec3(hw, hh, hd),   vec3(hw, -hh, hd),
      vec3(hw, -hh, -hd),  vec3(-hw, hh, hd),  vec3(-hw, hh, -hd),
      vec3(-hw, -hh, -hd), vec3(-hw, -hh, hd),
  };
  vector<uint32_t> indices = {
      0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 4, 1, 0, 4, 0, 5,
      6, 3, 2, 6, 2, 7, 1, 4, 7, 1, 7, 2, 5, 0, 3, 5, 3, 6,
  };
  return make_unique<Geometry>(move(vertices), move(indices));
}
