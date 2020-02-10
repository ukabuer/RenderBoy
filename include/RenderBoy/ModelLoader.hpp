#pragma once
#include <RenderBoy/Model.hpp>
#include <memory>
#include <string>

namespace RB {

class IModelLoader;

class ModelLoader {
public:
  ModelLoader() = delete;
  explicit ModelLoader(const std::string &path);

  ModelLoader(const ModelLoader &) = default;
  ModelLoader(ModelLoader &&) = default;
  auto operator=(const ModelLoader &) -> ModelLoader & = default;
  auto operator=(ModelLoader &&) -> ModelLoader & = default;

  ~ModelLoader();

  auto load() -> Model &;

private:
  std::shared_ptr<IModelLoader> impl;
};

} // namespace RB
