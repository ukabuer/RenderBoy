#pragma once

#include <string>

struct Texture {
  unsigned int id;
  unsigned char *data;
  size_t width;
  size_t height;
  std::string type;
  std::string path;
};