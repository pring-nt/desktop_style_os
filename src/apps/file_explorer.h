#ifndef CSOPESY_SRC_APPS_FILE_EXPLORER_H_
#define CSOPESY_SRC_APPS_FILE_EXPLORER_H_

#include "imgui.h"

#include "apps/app_window.h"

namespace csopesy::apps {

// Unique screen #1: a folder tree and a file list.
class FileExplorer : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{760.0F, 460.0F};

  FileExplorer() : AppWindow("File Explorer", kDefaultSize) {}

 protected:
  void Draw() override;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_FILE_EXPLORER_H_
