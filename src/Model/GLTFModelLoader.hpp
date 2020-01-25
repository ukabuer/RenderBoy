#pragma once
#include "Model/IModelLoader.hpp"

namespace RB {

class GLTFModelLoader final : public IModelLoader {
public:
  auto load() -> Model override;
};

} // namespace RB
