#ifndef CSOPESY_SRC_CORE_APP_H_
#define CSOPESY_SRC_CORE_APP_H_

#include "core/clock.h"
#include "core/state_machine.h"
#include "core/theme.h"
#include "shell/window_manager.h"

namespace csopesy::core {

// Owns the GLFW window, the ImGui context and every top-level object, and
// runs the frame loop. However the loop ends (PWR or the OS close button),
// the same cleanup path runs: resources are released in reverse order of
// creation.
class App {
 public:
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
  shell::WindowManager window_manager_;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_APP_H_
