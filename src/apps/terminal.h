#ifndef CSOPESY_SRC_APPS_TERMINAL_H_
#define CSOPESY_SRC_APPS_TERMINAL_H_

#include "imgui.h"

#include "apps/app_window.h"

namespace csopesy::apps {

// Unique screen #2: a command prompt with placeholder commands.
class Terminal : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{680.0F, 420.0F};

  Terminal() : AppWindow("Terminal", kDefaultSize) {}

 protected:
  void Draw() override;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_TERMINAL_H_
