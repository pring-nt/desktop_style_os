#ifndef CSOPESY_SRC_SHELL_DESKTOP_H_
#define CSOPESY_SRC_SHELL_DESKTOP_H_

#include "imgui.h"

#include "core/clock.h"

namespace csopesy::shell {

// An axis-aligned rectangle in screen pixels.
struct ScreenRect {
  ImVec2 min;
  ImVec2 max;
};

// Where the clock panel goes for a viewport: anchored to its top-right
// corner, so it follows the corner when the window is resized.
[[nodiscard]] ScreenRect ClockPanelRect(const ScreenRect& viewport,
                                        ImVec2 text_size);

// The desktop's base layer: the wallpaper, then the clock. Both go on ImGui's
// background draw list, which is drawn under every window, so they are always
// the first layers of the frame.
class Desktop {
 public:
  static void Draw(const core::Clock& clock);

 private:
  static void DrawWallpaper(ImDrawList& draw_list, const ScreenRect& viewport);
  static void DrawClock(ImDrawList& draw_list, const ScreenRect& viewport,
                        const core::Clock& clock);
};

}  // namespace csopesy::shell

#endif  // CSOPESY_SRC_SHELL_DESKTOP_H_
