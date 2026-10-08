#ifndef CSOPESY_SRC_SHELL_WINDOW_MANAGER_H_
#define CSOPESY_SRC_SHELL_WINDOW_MANAGER_H_

#include <vector>

#include "apps/app_window.h"

namespace csopesy::shell {

// Keeps the list of app windows and decides what a taskbar click means.
// Tracks the active window (the front-most app window) itself, because a
// click on the taskbar moves ImGui's focus to the taskbar.
class WindowManager {
 public:
  // Registers a window and gives it a staggered first-use position. The
  // window is borrowed and must outlive the manager.
  void Add(apps::AppWindow& window);

  // Taskbar click: closed -> open and focus; open and active -> minimize;
  // minimized or behind another window -> restore and focus.
  void ToggleFromTaskbar(apps::AppWindow& window);

  // Draws every open window and updates which one is active.
  void RenderAll(const apps::WorkArea& area);

  // Running means open, even when minimized; the taskbar shows an indicator.
  [[nodiscard]] static bool IsRunning(const apps::AppWindow& window) {
    return window.is_open();
  }
  [[nodiscard]] bool IsActive(const apps::AppWindow& window) const;

  [[nodiscard]] const std::vector<apps::AppWindow*>& windows() const {
    return windows_;
  }

 private:
  void Activate(apps::AppWindow& window);

  std::vector<apps::AppWindow*> windows_;
  apps::AppWindow* active_ = nullptr;
};

}  // namespace csopesy::shell

#endif  // CSOPESY_SRC_SHELL_WINDOW_MANAGER_H_
