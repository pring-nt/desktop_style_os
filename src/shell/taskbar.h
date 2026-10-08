#ifndef CSOPESY_SRC_SHELL_TASKBAR_H_
#define CSOPESY_SRC_SHELL_TASKBAR_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "imgui.h"

#include "apps/app_window.h"
#include "shell/desktop.h"
#include "shell/window_manager.h"

namespace csopesy::shell {

// The picture drawn on an app's taskbar button.
enum class TaskbarIcon : std::uint8_t { kFolder, kTerminal, kActivity };

// The taskbar strip for a viewport: full width, pinned to the bottom.
[[nodiscard]] ScreenRect TaskbarRect(const ScreenRect& viewport);

// The part of the viewport app windows may use: everything above the taskbar.
[[nodiscard]] apps::WorkArea WorkAreaAboveTaskbar(const ScreenRect& viewport);

// Where the app button at `index` goes, counting from the left.
[[nodiscard]] ScreenRect TaskbarButtonRect(const ScreenRect& taskbar,
                                           std::size_t index);

// The fixed bottom panel. It is placed from the main viewport every frame and
// kept in front of every app window. It only reports clicks; WindowManager
// decides what a click does.
class Taskbar {
 public:
  // Adds an app button after the existing ones. The window is borrowed and
  // must outlive the taskbar.
  void Pin(apps::AppWindow& window, TaskbarIcon icon);

  void Draw(WindowManager& window_manager) const;

 private:
  struct AppButton {
    apps::AppWindow* window;
    TaskbarIcon icon;
  };

  static void DrawIcon(ImDrawList& draw_list, TaskbarIcon icon,
                       const ScreenRect& button);

  std::vector<AppButton> buttons_;
};

}  // namespace csopesy::shell

#endif  // CSOPESY_SRC_SHELL_TASKBAR_H_
