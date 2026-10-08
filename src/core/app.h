#ifndef CSOPESY_SRC_CORE_APP_H_
#define CSOPESY_SRC_CORE_APP_H_

#include "apps/file_explorer.h"
#include "apps/task_manager.h"
#include "apps/terminal.h"
#include "core/clock.h"
#include "core/state_machine.h"
#include "core/theme.h"
#include "shell/desktop.h"
#include "shell/taskbar.h"
#include "shell/window_manager.h"

namespace csopesy::core {

// Owns the GLFW window, the ImGui context and every top-level object, and
// runs the frame loop. However the loop ends (PWR or the OS close button),
// the same cleanup path runs: resources are released in reverse order of
// creation.
class App {
 public:
  // Registers the app windows with the window manager and the taskbar.
  App();

  // Opens the window and runs until it closes. Returns the process exit code.
  [[nodiscard]] int Run();

 private:
  // Applies input and elapsed time to the state machine.
  void Update(Seconds elapsed);
  // Draws the current state.
  void Render();

  StateMachine state_machine_;
  Clock clock_;
  Theme theme_;
  shell::Desktop desktop_;
  apps::FileExplorer file_explorer_;
  apps::Terminal terminal_;
  apps::TaskManager task_manager_;
  shell::WindowManager window_manager_;
  shell::Taskbar taskbar_;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_APP_H_
