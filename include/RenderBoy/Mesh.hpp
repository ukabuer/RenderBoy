#pragma once
#include "RenderBoy/Geometry.hpp"

struct Mesh {
  std::vector<Geometry> geometries;
  Eigen::Matrix4f model_matrix = Eigen::Matrix4f::Identity();

  void set_model_matrix(const Eigen::Matrix4f &matrix) {
    model_matrix = matrix;
  }
};
