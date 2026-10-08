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

  // Taskbar: a dark, semi-opaque strip along the bottom with a thin top
  // border.
  static constexpr float kTaskbarHeight = 56.0F;
  static constexpr ImVec4 kTaskbarColor{0.05F, 0.06F, 0.09F, 0.88F};
  static constexpr ImVec4 kTaskbarBorderColor{1.0F, 1.0F, 1.0F, 0.14F};
  static constexpr float kTaskbarBorderThickness = 1.0F;
  static constexpr float kTaskbarPadding = 10.0F;

  // Taskbar app buttons: rounded squares with an icon drawn on top.
  static constexpr float kTaskbarButtonSize = 44.0F;
  static constexpr float kTaskbarButtonSpacing = 6.0F;
  static constexpr float kTaskbarButtonRounding = 8.0F;
  static constexpr ImVec4 kTaskbarButtonHoverColor{1.0F, 1.0F, 1.0F, 0.10F};
  static constexpr ImVec4 kTaskbarButtonActiveColor{1.0F, 1.0F, 1.0F, 0.06F};

  // Running-app indicator under a taskbar button: a short grey bar while the
  // app is open, a longer accent bar while its window is active.
  static constexpr float kIndicatorHeight = 3.0F;
  static constexpr float kIndicatorRounding = 1.5F;
  static constexpr float kIndicatorRunningWidth = 6.0F;
  static constexpr float kIndicatorActiveWidth = 16.0F;
  static constexpr ImVec4 kIndicatorRunningColor{0.62F, 0.64F, 0.70F, 1.0F};
  static constexpr ImVec4 kIndicatorActiveColor{0.38F, 0.68F, 1.0F, 1.0F};

  static constexpr ImVec4 kFolderIconColor{0.98F, 0.78F, 0.30F, 1.0F};
  static constexpr ImVec4 kFolderTabColor{0.85F, 0.62F, 0.18F, 1.0F};
  static constexpr ImVec4 kTerminalIconColor{0.35F, 0.92F, 0.45F, 1.0F};
  static constexpr ImVec4 kTerminalIconBackground{0.07F, 0.08F, 0.10F, 1.0F};
  static constexpr ImVec4 kActivityIconColor{0.38F, 0.72F, 1.0F, 1.0F};

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
