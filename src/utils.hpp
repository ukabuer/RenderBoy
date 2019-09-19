#pragma once
#include "Camera/Camera.hpp"
#include "Scene.hpp"
#include <memory>
#include <string>
#include <utility>

std::pair<std::unique_ptr<Camera>, std::unique_ptr<Scene>>
load_config(const std::string &filename);
