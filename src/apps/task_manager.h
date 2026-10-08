#ifndef CSOPESY_SRC_APPS_TASK_MANAGER_H_
#define CSOPESY_SRC_APPS_TASK_MANAGER_H_

#include "imgui.h"

#include "apps/app_window.h"

namespace csopesy::apps {

// The Windows-style Task Manager, Processes view.
class TaskManager : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{720.0F, 480.0F};

  TaskManager() : AppWindow("Task Manager", kDefaultSize) {}

 protected:
  void Draw() override;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_TASK_MANAGER_H_
