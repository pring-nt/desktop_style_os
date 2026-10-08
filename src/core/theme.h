#ifndef CSOPESY_SRC_CORE_THEME_H_
#define CSOPESY_SRC_CORE_THEME_H_

#include "imgui.h"

namespace csopesy::core {

// Colors, fonts, rounding and spacing for the whole app, applied once at
// startup. Features take their styling from here instead of hard-coding it.
class Theme {
 public:
  // Unscaled shell font size, in pixels.
  static constexpr float kShellFontSize = 16.0F;
  // ProggyClean is pixel-exact at 13 px; boot screens scale it by whole
  // multiples.
  static constexpr float kBootFontSize = 13.0F;

  // Desktop wallpaper fallback: a vertical gradient, top to bottom.
  static constexpr ImVec4 kWallpaperTopColor{0.11F, 0.23F, 0.45F, 1.0F};
  static constexpr ImVec4 kWallpaperBottomColor{0.02F, 0.05F, 0.14F, 1.0F};

  // Clock panel in the desktop's top-right corner.
  static constexpr ImVec4 kClockPanelColor{0.0F, 0.0F, 0.0F, 0.55F};
  static constexpr ImVec4 kClockTextColor{0.95F, 0.95F, 0.97F, 1.0F};
  static constexpr ImVec2 kClockPanelPadding{12.0F, 6.0F};
  static constexpr float kClockPanelRounding = 8.0F;
  static constexpr float kClockPanelMargin = 12.0F;

  // Loads the fonts and sets the ImGui style. Call once, after
  // ImGui::CreateContext() and before the first frame.
  void Apply();

  // ProggyForever, Dear ImGui's scalable default font. The default font for
  // every window.
  [[nodiscard]] ImFont* shell_font() const { return shell_font_; }
  // ProggyClean, the pixel font for the BIOS and splash screens.
  [[nodiscard]] ImFont* boot_font() const { return boot_font_; }

 private:
  ImFont* shell_font_ = nullptr;
  ImFont* boot_font_ = nullptr;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_THEME_H_
