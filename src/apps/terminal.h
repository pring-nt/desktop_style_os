#ifndef CSOPESY_SRC_APPS_TERMINAL_H_
#define CSOPESY_SRC_APPS_TERMINAL_H_

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"

#include "apps/app_window.h"
#include "core/clock.h"
#include "data/dummy_process_table.h"
#include "data/terminal_commands.h"

namespace csopesy::apps {

// Unique screen #2, a cmd.exe-style prompt: a black scrollback of green text
// above a single input line. Commands come from data::RunCommand; Up and
// Down browse earlier commands.
class Terminal : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{680.0F, 420.0F};
  static constexpr std::size_t kInputCapacity = 256;

  // Both are borrowed and must outlive the window.
  Terminal(const core::Clock& clock, const data::DummyProcessTable& processes);

  // Runs a line as if it were typed and submitted: echoes it after the
  // prompt, then applies the command's output and effect.
  void Submit(std::string_view input);

  [[nodiscard]] const std::vector<std::string>& scrollback() const {
    return scrollback_;
  }

 protected:
  void Draw() override;

 private:
  static int HandleInputCallback(ImGuiInputTextCallbackData* data);
  void DrawScrollback();
  void DrawInputLine();

  const core::Clock* clock_;
  const data::DummyProcessTable* processes_;
  std::vector<std::string> scrollback_;
  data::CommandHistory history_;
  std::array<char, kInputCapacity> input_{};
  bool scroll_to_bottom_ = false;
  bool focus_input_ = true;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_TERMINAL_H_
