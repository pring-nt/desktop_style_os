#include "apps/task_manager.h"

#include <cstddef>
// doctest prints std::string_view operands with operator<<.
#include <ostream>  // IWYU pragma: keep

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"
#include "data/dummy_process_table.h"

namespace csopesy::apps {
namespace {

using testing::HeadlessImGui;

constexpr double kCpu = 3.4;
constexpr double kMemory = 128.6;
constexpr double kTotalJustBelowHalf = 21.49;
constexpr double kTotalAtHalf = 21.5;
constexpr std::size_t kApps = 6;
constexpr std::size_t kBackground = 14;
constexpr int kFrames = 3;
constexpr WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 664.0F),
};

TEST_CASE("Task Manager formats cells like Windows does") {
  CHECK(FormatCpu(kCpu) == "3.4%");
  CHECK(FormatCpu(0.0) == "0.0%");
  CHECK(FormatMemory(kMemory) == "128.6 MB");
}

TEST_CASE("Task Manager rounds column totals to whole percents") {
  CHECK(FormatTotal(kTotalJustBelowHalf) == "21%");
  CHECK(FormatTotal(kTotalAtHalf) == "22%");
  CHECK(FormatTotal(data::kMaxPercent) == "100%");
}

TEST_CASE("Task Manager labels the process groups with their sizes") {
  CHECK(FormatGroupHeader(data::ProcessGroup::kApp, kApps) == "Apps (6)");
  CHECK(FormatGroupHeader(data::ProcessGroup::kBackground, kBackground) ==
        "Background processes (14)");
}

TEST_CASE("Task Manager draws its processes table") {
  const HeadlessImGui imgui;
  const data::DummyProcessTable table;
  TaskManager task_manager(table);
  task_manager.Open();
  for (int frame = 0; frame < kFrames; ++frame) {
    HeadlessImGui::BeginFrame();
    task_manager.Render(kArea);
    HeadlessImGui::EndFrame();
  }
  CHECK(task_manager.is_open());
  CHECK(task_manager.selected_process().empty());
}

}  // namespace
}  // namespace csopesy::apps
