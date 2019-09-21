#include "utils.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Model.hpp"
#include <cpptoml.h>
#include <iostream>

using namespace std;

auto RenderConfig::Load(const string &filename) -> RenderConfig {
  auto result = RenderConfig{};
  auto &scene = result.scene;
  auto &camera = result.camera;

  try {
    auto config = cpptoml::parse_file(filename);

    auto cameraConfig = config->get_table("camera");
    if (!cameraConfig) {
      throw cpptoml::parse_exception("no camera");
    }

    auto position = cameraConfig->get_array_of<double>("position")
                        .value_or(initializer_list<double>{1.0, 1.0, 1.0});
    auto width = cameraConfig->get_as<unsigned>("width").value_or(800u);
    auto height = cameraConfig->get_as<unsigned>("height").value_or(600u);
    auto fov = cameraConfig->get_as<double>("fov").value_or(60.0);
    auto near = cameraConfig->get_as<double>("near").value_or(0.1);
    auto far = cameraConfig->get_as<double>("far").value_or(1000.0);
    camera = make_unique<PerspectiveCamera>(static_cast<float>(fov), width,
                                            height, static_cast<float>(near),
                                            static_cast<float>(far));
    camera->setPosition(static_cast<float>(position[0]),
                        static_cast<float>(position[1]),
                        static_cast<float>(position[2]));

    auto lightConfigs = config->get_table_array("lights");
    if (!lightConfigs) {
      throw cpptoml::parse_exception("no lights");
    }

    for (const auto &val : *lightConfigs) {
      auto lightConfig = val->as_table();
      auto type = lightConfig->get_as<string>("type").value_or("point");
      if (type == "point") {
        auto light = Light();
        auto position = config->get_array_of<double>("position")
                            .value_or(initializer_list<double>{1.0, 1.0, 1.0});
        light.position = {static_cast<float>(position[0]),
                          static_cast<float>(position[1]),
                          static_cast<float>(position[2])};

        auto color = config->get_array_of<double>("color").value_or(
            initializer_list<double>{1.0, 1.0, 1.0});
        light.color = {static_cast<float>(color[0]),
                       static_cast<float>(color[1]),
                       static_cast<float>(color[2])};
        scene.lights.emplace_back(light);
      }
    }

    auto modelConfigs = config->get_table_array("models");
    if (!cameraConfig) {
      throw cpptoml::parse_exception("no models");
    }

    for (const auto &val : *modelConfigs) {
      auto modelConfig = val->as_table();
      auto path = modelConfig->get_as<string>("path");
      if (!path) {
        throw cpptoml::parse_exception("The path of model is not specificed");
      }

      auto model = Model::Load(*path);
      for (auto &mesh : model.meshes) {
        scene.meshes.emplace_back(mesh);
      }
    }
  } catch (cpptoml::parse_exception error) {
    cerr << error.what() << endl;
  }

  return result;
}