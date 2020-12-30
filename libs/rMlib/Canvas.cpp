#include "Canvas.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace rmlib {

std::optional<ImageCanvas>
ImageCanvas::load(const char* path, int components) {
  Canvas result;
  result.memory = stbi_load(
    path, &result.width, &result.height, &result.components, components);
  if (result.memory == nullptr) {
    return std::nullopt;
  }

  if (components != 0) {
    result.components = components;
  }

  return ImageCanvas{ result };
}

void
ImageCanvas::release() {
  if (canvas.memory != nullptr) {
    stbi_image_free(canvas.memory);
  }
  canvas.memory = nullptr;
}

} // namespace rmlib