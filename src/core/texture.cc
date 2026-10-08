#include "core/texture.h"

#include <climits>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "imgui.h"
#include <glad/gl.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include <stb_image.h>

namespace csopesy::core {

namespace {

constexpr int kRgbaChannels = 4;

struct StbiDeleter {
  void operator()(stbi_uc* pixels) const { stbi_image_free(pixels); }
};
using PixelsPtr = std::unique_ptr<stbi_uc, StbiDeleter>;

[[nodiscard]] std::vector<stbi_uc> ReadFile(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return {};
  }
  return {std::istreambuf_iterator<char>(file),
          std::istreambuf_iterator<char>()};
}

}  // namespace

std::optional<Texture> Texture::LoadFromFile(
    const std::filesystem::path& path) {
  const std::vector<stbi_uc> bytes = ReadFile(path);
  if (bytes.empty() || bytes.size() > static_cast<std::size_t>(INT_MAX)) {
    return std::nullopt;
  }
  int width = 0;
  int height = 0;
  int channels = 0;
  const PixelsPtr pixels(
      stbi_load_from_memory(bytes.data(), static_cast<int>(bytes.size()),
                            &width, &height, &channels, kRgbaChannels));
  if (!pixels) {
    return std::nullopt;
  }

  GLuint gl_id = 0;
  glGenTextures(1, &gl_id);
  glBindTexture(GL_TEXTURE_2D, gl_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels.get());
  glBindTexture(GL_TEXTURE_2D, 0);
  return Texture(gl_id,
                 ImVec2(static_cast<float>(width), static_cast<float>(height)));
}

Texture::Texture(unsigned int gl_id, ImVec2 size) : id_(gl_id), size_(size) {}

Texture::~Texture() { Release(); }

Texture::Texture(Texture&& other) noexcept
    : id_(std::exchange(other.id_, 0)), size_(other.size_) {}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (this != &other) {
    Release();
    id_ = std::exchange(other.id_, 0);
    size_ = other.size_;
  }
  return *this;
}

void Texture::Release() {
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
    id_ = 0;
  }
}

}  // namespace csopesy::core
