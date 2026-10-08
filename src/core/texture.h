#ifndef CSOPESY_SRC_CORE_TEXTURE_H_
#define CSOPESY_SRC_CORE_TEXTURE_H_

#include <filesystem>
#include <optional>

#include "imgui.h"

namespace csopesy::core {

// An RGBA OpenGL texture decoded from an image file. Move-only; frees the GL
// texture on destruction, so it must not outlive the GL context.
class Texture {
 public:
  // Needs a current GL context. Returns nullopt if the file is missing or
  // can't be decoded.
  [[nodiscard]] static std::optional<Texture> LoadFromFile(
      const std::filesystem::path& path);

  ~Texture();
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;
  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;

  [[nodiscard]] ImTextureID id() const { return id_; }
  // Width and height in pixels.
  [[nodiscard]] ImVec2 size() const { return size_; }

 private:
  Texture(unsigned int gl_id, ImVec2 size);
  void Release();

  unsigned int id_ = 0;
  ImVec2 size_;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_TEXTURE_H_
