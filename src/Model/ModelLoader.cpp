#include "Model/GLTFModelLoader.hpp"
#include <RenderBoy/ModelLoader.hpp>
#include <exception>

using namespace std;

namespace RB {

ModelLoader::~ModelLoader() = default;

ModelLoader::ModelLoader(const string &path) {
  auto ext_idx = path.find_last_of('.');
  if (ext_idx == string::npos) {
    throw runtime_error("unknow model format");
  }

  auto ext = path.substr(ext_idx);
  if (ext == ".gltf" || ext == ".glb") {
    impl.reset(new GLTFModelLoader);
  } else {
    throw runtime_error("unsupported model format");
  }
  impl->set_path(path);
}

auto ModelLoader::load() -> Model & { return impl->load(); }

auto ModelLoader::get_extends() const -> BoundingBox {
  return impl->get_extends();
};

} // namespace RB