#ifndef CSOPESY_SRC_CORE_THEME_H_
#define CSOPESY_SRC_CORE_THEME_H_

#include <array>
#include <filesystem>
#include <string_view>
#include <vector>

#include "imgui.h"

namespace csopesy::core {

// Colors, fonts, rounding and spacing for the whole app, applied once at
// startup. Features take their styling from here instead of hard-coding it.
class Theme {
 public:
  // The shell font, relative to the executable.
  static constexpr std::string_view kShellFontPath =
      "assets/fonts/Roboto-Medium.ttf";
  // Unscaled shell font size, in pixels.
  static constexpr float kShellFontSize = 16.0F;
  // ProggyClean is pixel-exact at 13 px; boot screens scale it by whole
  // multiples.
  static constexpr float kBootFontSize = 13.0F;

  // BIOS, splash and shutdown screens: the pixel font at twice its size.
  static constexpr float kBootTextScale = 2.0F;
  static constexpr float kBootLineSpacing = 1.25F;
  static constexpr ImVec2 kBootMargin{28.0F, 24.0F};
  static constexpr ImVec4 kBootBackgroundColor{0.0F, 0.0F, 0.0F, 1.0F};
  static constexpr ImVec4 kBootTextColor{0.78F, 0.78F, 0.78F, 1.0F};
  static constexpr ImVec4 kBiosFunFactColor{0.98F, 0.86F, 0.35F, 1.0F};
  static constexpr ImVec4 kSplashLogoColor{0.33F, 0.60F, 1.0F, 1.0F};
  static constexpr ImVec4 kSplashSubtitleColor{0.55F, 0.58F, 0.65F, 1.0F};
  static constexpr ImVec4 kSplashLoadingColor{0.35F, 0.92F, 0.45F, 1.0F};

  // Desktop wallpaper fallback: a vertical gradient, top to bottom.
  static constexpr ImVec4 kWallpaperTopColor{0.11F, 0.23F, 0.45F, 1.0F};
  static constexpr ImVec4 kWallpaperBottomColor{0.02F, 0.05F, 0.14F, 1.0F};

  // Clock panel in the desktop's top-right corner.
  static constexpr ImVec4 kClockPanelColor{0.0F, 0.0F, 0.0F, 0.55F};
  static constexpr ImVec4 kClockTextColor{0.95F, 0.95F, 0.97F, 1.0F};
  static constexpr ImVec2 kClockPanelPadding{12.0F, 6.0F};
  static constexpr float kClockPanelRounding = 8.0F;
  static constexpr float kClockPanelMargin = 12.0F;

  // Desktop label in the top-left corner, styled like the clock panel.
  static constexpr float kDesktopLabelDotGap = 4.0F;
  // The status dot's radius as a share of the text height.
  static constexpr float kDesktopLabelDotScale = 0.25F;
  static constexpr ImVec4 kDesktopLabelDotColor{0.35F, 0.92F, 0.45F, 1.0F};

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

  // System tray buttons on the right of the taskbar.
  static constexpr ImVec2 kTrayButtonSize{52.0F, 32.0F};
  static constexpr float kTrayButtonSpacing = 4.0F;
  static constexpr ImVec4 kTrayTextColor{0.85F, 0.87F, 0.92F, 1.0F};
  static constexpr ImVec4 kPowerTextColor{0.96F, 0.36F, 0.36F, 1.0F};

  // VOL and NET popups open this far above their tray button.
  static constexpr float kTrayPopupGap = 8.0F;
  static constexpr float kVolumeSliderWidth = 180.0F;

  // The PWR confirmation dialog.
  static constexpr ImVec2 kDialogButtonSize{110.0F, 0.0F};

  // Task Manager processes table.
  static constexpr float kStatusColumnWidth = 90.0F;
  static constexpr float kNumberColumnWidth = 90.0F;
  static constexpr float kTaskManagerRowIndent = 14.0F;
  static constexpr ImVec4 kTaskManagerGroupColor{0.55F, 0.75F, 1.0F, 1.0F};
  // The sort arrow beside the sorted column's name.
  static constexpr float kSortArrowHalfWidth = 4.0F;
  static constexpr float kSortArrowHalfHeight = 2.5F;
  static constexpr float kSortArrowInset = 4.0F;
  // CPU and Memory cell shading, blended over the dark table from idle to
  // heavy use.
  static constexpr ImVec4 kUsageHeatLowColor{1.0F, 0.92F, 0.55F, 0.10F};
  static constexpr ImVec4 kUsageHeatHighColor{1.0F, 0.62F, 0.08F, 0.60F};

  // File Explorer panes and columns.
  static constexpr float kFolderTreeWidth = 170.0F;
  static constexpr float kFileTypeColumnWidth = 160.0F;
  static constexpr float kFileSizeColumnWidth = 90.0F;
  static constexpr float kFileDateColumnWidth = 150.0F;

  // Terminal: green-on-black, like a classic console.
  static constexpr ImVec4 kTerminalBackground{0.02F, 0.02F, 0.02F, 1.0F};
  static constexpr ImVec4 kTerminalTextColor{0.72F, 0.95F, 0.72F, 1.0F};

  // Minesweeper, in the classic grey look with bevelled hidden cells.
  static constexpr float kMineCellSize = 24.0F;
  static constexpr float kMineBevel = 2.0F;
  static constexpr float kMineStroke = 1.5F;
  static constexpr ImVec4 kMineHiddenColor{0.75F, 0.75F, 0.75F, 1.0F};
  static constexpr ImVec4 kMineBevelLightColor{1.0F, 1.0F, 1.0F, 1.0F};
  static constexpr ImVec4 kMineBevelDarkColor{0.50F, 0.50F, 0.50F, 1.0F};
  static constexpr ImVec4 kMineRevealedColor{0.75F, 0.75F, 0.75F, 1.0F};
  static constexpr ImVec4 kMineGridColor{0.50F, 0.50F, 0.50F, 1.0F};
  static constexpr ImVec4 kMineExplodedColor{1.0F, 0.0F, 0.0F, 1.0F};
  static constexpr ImVec4 kMineFlagColor{0.90F, 0.0F, 0.0F, 1.0F};
  static constexpr ImVec4 kMineBlackColor{0.0F, 0.0F, 0.0F, 1.0F};
  static constexpr ImVec4 kMineHighlightColor{1.0F, 1.0F, 1.0F, 1.0F};
  static constexpr ImVec4 kMineCounterBackground{0.0F, 0.0F, 0.0F, 1.0F};
  static constexpr ImVec4 kMineCounterColor{1.0F, 0.15F, 0.10F, 1.0F};
  // The classic colors for 1 to 8 adjacent mines.
  static constexpr std::array<ImVec4, 8> kMineNumberColors{
      ImVec4(0.0F, 0.0F, 1.0F, 1.0F),  ImVec4(0.0F, 0.50F, 0.0F, 1.0F),
      ImVec4(1.0F, 0.0F, 0.0F, 1.0F),  ImVec4(0.0F, 0.0F, 0.50F, 1.0F),
      ImVec4(0.50F, 0.0F, 0.0F, 1.0F), ImVec4(0.0F, 0.50F, 0.50F, 1.0F),
      ImVec4(0.0F, 0.0F, 0.0F, 1.0F),  ImVec4(0.50F, 0.50F, 0.50F, 1.0F),
  };

  static constexpr ImVec4 kFolderIconColor{0.98F, 0.78F, 0.30F, 1.0F};
  static constexpr ImVec4 kFolderTabColor{0.85F, 0.62F, 0.18F, 1.0F};
  static constexpr ImVec4 kTerminalIconColor{0.35F, 0.92F, 0.45F, 1.0F};
  static constexpr ImVec4 kTerminalIconBackground{0.07F, 0.08F, 0.10F, 1.0F};
  static constexpr ImVec4 kActivityIconColor{0.38F, 0.72F, 1.0F, 1.0F};
  static constexpr ImVec4 kMineIconColor{0.86F, 0.88F, 0.92F, 1.0F};
  static constexpr ImVec4 kMineIconShine{0.07F, 0.08F, 0.10F, 1.0F};

  // Loads the fonts and sets the ImGui style. Call once, after
  // ImGui::CreateContext() and before the first frame. The shell font is read
  // from `shell_font_path`; if it is empty or can't be read, Dear ImGui's
  // scalable default font (ProggyForever) is used instead.
  void Apply(const std::filesystem::path& shell_font_path = {});

  // Roboto (or the fallback). The default font for every window.
  [[nodiscard]] ImFont* shell_font() const { return shell_font_; }
  // ProggyClean, the pixel font for the BIOS and splash screens.
  [[nodiscard]] ImFont* boot_font() const { return boot_font_; }

 private:
  // The font file's bytes; ImGui reads them from here, so they live as long
  // as the theme.
  std::vector<unsigned char> shell_font_data_;
  ImFont* shell_font_ = nullptr;
  ImFont* boot_font_ = nullptr;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_THEME_H_
