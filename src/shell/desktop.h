#ifndef CSOPESY_SRC_SHELL_DESKTOP_H_
#define CSOPESY_SRC_SHELL_DESKTOP_H_

#include <filesystem>
#include <optional>
#include <string_view>

#include "imgui.h"

#include "core/clock.h"
#include "core/texture.h"

namespace csopesy::shell {

// An axis-aligned rectangle in screen pixels.
struct ScreenRect {
  ImVec2 min;
  ImVec2 max;
};

// A region of a texture in normalized [0, 1] coordinates.
struct UvRect {
  ImVec2 min;
  ImVec2 max;
};

// The main viewport (the whole application window) in screen pixels.
[[nodiscard]] ScreenRect MainViewportRect();

// Where the clock panel goes for a viewport: anchored to its top-right
// corner, so it follows the corner when the window is resized.
[[nodiscard]] ScreenRect ClockPanelRect(const ScreenRect& viewport,
                                        ImVec2 text_size);

// The status label in the desktop's top-left corner.
inline constexpr std::string_view kDesktopLabel =
    "CSOPESY OS v1.0 - System Online";

// Where the desktop label panel goes: anchored to the viewport's top-left
// corner, mirroring the clock panel.
[[nodiscard]] ScreenRect DesktopLabelRect(const ScreenRect& viewport,
                                          ImVec2 content_size);

// The centered part of an image that covers the viewport while keeping the
// image's aspect ratio: the overflowing axis is cropped equally on both sides.
[[nodiscard]] UvRect CoverUv(ImVec2 image_size, const ScreenRect& viewport);

// The desktop's base layer: the wallpaper, then the label and the clock. Both
// go on ImGui's background draw list, which is drawn under every window, so
// they are always the first layers of the frame.
class Desktop {
 public:
  // Needs a current GL context. If the image can't be loaded, the gradient is
  // drawn instead.
  void LoadWallpaper(const std::filesystem::path& path);
  // Frees the wallpaper texture; call before the GL context is destroyed.
  void ReleaseWallpaper();
  [[nodiscard]] bool has_wallpaper() const { return wallpaper_.has_value(); }

  void Draw(const core::Clock& clock) const;

 private:
  void DrawWallpaper(ImDrawList& draw_list, const ScreenRect& viewport) const;
  static void DrawGradient(ImDrawList& draw_list, const ScreenRect& viewport);
  static void DrawLabel(ImDrawList& draw_list, const ScreenRect& viewport);
  static void DrawClock(ImDrawList& draw_list, const ScreenRect& viewport,
                        const core::Clock& clock);

  std::optional<core::Texture> wallpaper_;
};

}  // namespace csopesy::shell

#endif  // CSOPESY_SRC_SHELL_DESKTOP_H_
