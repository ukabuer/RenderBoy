#pragma once

#include <string>

struct Texture {
  unsigned char *data = nullptr;
  size_t width = 0;
  size_t height = 0;
  size_t channels = 3;
  std::string type;
  std::string path;
};