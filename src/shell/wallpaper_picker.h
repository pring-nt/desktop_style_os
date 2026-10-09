#ifndef CSOPESY_SRC_SHELL_WALLPAPER_PICKER_H_
#define CSOPESY_SRC_SHELL_WALLPAPER_PICKER_H_

#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "imgui.h"

#include "apps/app_window.h"
#include "core/texture.h"

namespace csopesy::shell {

// The "Wallpaper" window opened from the desktop's right-click menu: a grid
// of thumbnails, one per image in the wallpaper folder, plus a "Gradient"
// tile. It only reports what was clicked; App applies and saves the choice.
class WallpaperPicker : public apps::AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{580.0F, 330.0F};

  explicit WallpaperPicker(std::filesystem::path directory)
      : AppWindow("Wallpaper", kDefaultSize),
        directory_(std::move(directory)) {}

  // Lists the wallpaper folder again and drops any loaded thumbnails. Call
  // before showing the window so new images appear.
  void Rescan();
  // Loads a thumbnail texture for every listed image. Needs a current GL
  // context, and must run before this frame draws the picker.
  void LoadThumbnails();
  // Frees the thumbnail textures; call before the GL context is destroyed.
  void ReleaseThumbnails();

  [[nodiscard]] const std::string& current() const { return current_; }
  void set_current(std::string name) { current_ = std::move(name); }
  [[nodiscard]] std::vector<std::string> wallpapers() const;

  // The wallpaper clicked since the last call, if any.
  [[nodiscard]] std::optional<std::string> TakeSelection();

 protected:
  void Draw() override;

 private:
  struct Thumbnail {
    std::string name;
    std::optional<core::Texture> texture;
  };

  // Draws one tile and returns true when it was clicked.
  [[nodiscard]] bool DrawTile(const std::string& name,
                              const core::Texture* texture) const;

  std::filesystem::path directory_;
  std::vector<Thumbnail> thumbnails_;
  std::string current_;
  std::optional<std::string> selection_;
};

}  // namespace csopesy::shell

#endif  // CSOPESY_SRC_SHELL_WALLPAPER_PICKER_H_
