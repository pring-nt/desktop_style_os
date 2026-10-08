#ifndef CSOPESY_SRC_SHELL_TASKBAR_H_
#define CSOPESY_SRC_SHELL_TASKBAR_H_

#include "apps/app_window.h"
#include "shell/desktop.h"

namespace csopesy::shell {

// The taskbar strip for a viewport: full width, pinned to the bottom.
[[nodiscard]] ScreenRect TaskbarRect(const ScreenRect& viewport);

// The part of the viewport app windows may use: everything above the taskbar.
[[nodiscard]] apps::WorkArea WorkAreaAboveTaskbar(const ScreenRect& viewport);

// The fixed bottom panel. It is placed from the main viewport every frame
// and kept in front of every app window.
class Taskbar {
 public:
  static void Draw();
};

}  // namespace csopesy::shell

#endif  // CSOPESY_SRC_SHELL_TASKBAR_H_
