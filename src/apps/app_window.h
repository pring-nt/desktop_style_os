#ifndef CSOPESY_SRC_APPS_APP_WINDOW_H_
#define CSOPESY_SRC_APPS_APP_WINDOW_H_

#include <string>

#include "imgui.h"

namespace csopesy::apps {

// How much of a window must stay on screen when dragged sideways.
inline constexpr float kMinVisibleWidth = 64.0F;

// The desktop region app windows live in: the viewport above the taskbar.
struct WorkArea {
  ImVec2 min;
  ImVec2 max;
};

// Where a window at `position` may sit so it cannot be dragged below the
// taskbar or lost off screen: its title bar stays inside `area` vertically and
// a strip of it stays visible horizontally.
[[nodiscard]] ImVec2 ClampToWorkArea(ImVec2 position, ImVec2 size,
                                     const WorkArea& area,
                                     float title_bar_height);

// Base class for every app window (File Explorer, Terminal, Task Manager).
// Frames the window with a title bar, close and minimize buttons, then calls
// Draw() for the app's own contents. Open, minimized and focus state lives
// here, not in ImGui, so the taskbar and WindowManager can read and change
// it.
class AppWindow {
 public:
  AppWindow(std::string title, ImVec2 default_size);
  virtual ~AppWindow() = default;
  AppWindow(const AppWindow&) = delete;
  AppWindow& operator=(const AppWindow&) = delete;
  AppWindow(AppWindow&&) = delete;
  AppWindow& operator=(AppWindow&&) = delete;

  // Draws the window this frame if it is open and not minimized.
  void Render(const WorkArea& area);

  // Shows the window (restoring it if minimized) and brings it to the front.
  void Open();
  // Hides the window but keeps it running on the taskbar.
  void Minimize();
  void Close();

  [[nodiscard]] const std::string& title() const { return title_; }
  [[nodiscard]] bool is_open() const { return is_open_; }
  [[nodiscard]] bool is_minimized() const { return is_minimized_; }
  // Whether the window had keyboard focus during the last rendered frame.
  [[nodiscard]] bool is_focused() const { return is_focused_; }

  // Offset of the first-time position from the work area's top-left corner,
  // so windows opened for the first time do not stack exactly.
  void set_default_offset(ImVec2 default_offset) {
    default_offset_ = default_offset;
  }

 protected:
  // Draws the app's contents inside the window.
  virtual void Draw() = 0;

 private:
  // Returns true when the title-bar minimize button was clicked.
  static bool DrawMinimizeButton();

  std::string title_;
  ImVec2 default_size_;
  ImVec2 default_offset_;
  bool is_open_ = false;
  bool is_minimized_ = false;
  bool is_focused_ = false;
  bool focus_requested_ = false;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_APP_WINDOW_H_
