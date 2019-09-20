#pragma once
#include "Camera/Camera.hpp"
#include "Scene.hpp"
#include <memory>
#include <string>
#include <utility>

auto load_config(const std::string &filename)
    -> std::pair<std::unique_ptr<Camera>, std::unique_ptr<Scene>>;
