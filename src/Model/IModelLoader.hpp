#pragma once
#include <RenderBoy/Model.hpp>
#include <string>

namespace RB {

class IModelLoader {
public:
  ~IModelLoader() = default;

  void set_path(const std::string &_path) {
    this->path = _path;
    const char delimiter = '/';
    auto idx = _path.find_last_of(delimiter);
    if (idx == std::string::npos) {
      this->dir = ".";
    } else {
      this->dir = _path.substr(0, idx);
    }
  }

  virtual auto load() -> Model & = 0;

protected:
  std::string dir;
  std::string path;
};

} // namespace RB
