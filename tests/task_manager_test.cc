#include "apps/task_manager.h"

#include <cstddef>
#include <vector>
// doctest prints std::string_view operands with operator<<.
#include <ostream>  // IWYU pragma: keep

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"
#include "apps/sort_direction.h"
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
constexpr double kHalfShare = 0.5;
constexpr float kHalfHeat = 0.5F;
constexpr double kDoubleShare = 2.0;
constexpr float kOverHeat = 2.0F;
constexpr ProcessSort kUnsorted = ProcessSort();
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

TEST_CASE("UsageHeat scales usage into 0 to 1") {
  CHECK(UsageHeat(0.0, kCpuFullHeatPercent) == 0.0F);
  CHECK(UsageHeat(kCpuFullHeatPercent * kHalfShare, kCpuFullHeatPercent) ==
        doctest::Approx(kHalfHeat));
  CHECK(UsageHeat(kMemoryFullHeatMb * kDoubleShare, kMemoryFullHeatMb) == 1.0F);
  CHECK(UsageHeat(kCpu, 0.0) == 0.0F);
}

TEST_CASE("HeatColor runs from pale yellow to deep amber") {
  const ImVec4 idle = HeatColor(0.0F);
  const ImVec4 heavy = HeatColor(1.0F);
  const ImVec4 middle = HeatColor(kHalfHeat);
  CHECK(idle.w < middle.w);
  CHECK(middle.w < heavy.w);
  CHECK(idle.y > heavy.y);
  CHECK(HeatColor(kOverHeat).w == heavy.w);
}

TEST_CASE("Task Manager draws its processes table") {
  const HeadlessImGui imgui;
  data::DummyProcessTable table;
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

TEST_CASE("Clicking a new column sorts numbers heaviest first") {
  const ProcessSort cpu = ToggleSort(kUnsorted, ProcessColumn::kCpu);
  CHECK(cpu.column == ProcessColumn::kCpu);
  CHECK(cpu.direction == SortDirection::kDescending);
  const ProcessSort name = ToggleSort(cpu, ProcessColumn::kName);
  CHECK(name.column == ProcessColumn::kName);
  CHECK(name.direction == SortDirection::kAscending);
}

TEST_CASE("Clicking the sorted column flips its direction") {
  const ProcessSort once = ToggleSort(kUnsorted, ProcessColumn::kMemory);
  const ProcessSort twice = ToggleSort(once, ProcessColumn::kMemory);
  CHECK(twice.direction == SortDirection::kAscending);
}

TEST_CASE("Unsorted rows keep the table order within their group") {
  const data::DummyProcessTable table;
  const std::vector<const data::ProcessRow*> apps =
      RowsInGroup(table.rows(), data::ProcessGroup::kApp, kUnsorted);
  REQUIRE(apps.size() == kApps);
  CHECK(apps.front()->name == "csopesy_shell.exe");
}

TEST_CASE("Rows sort by CPU, heaviest first") {
  const data::DummyProcessTable table;
  const std::vector<const data::ProcessRow*> apps = RowsInGroup(
      table.rows(), data::ProcessGroup::kApp,
      {.column = ProcessColumn::kCpu, .direction = SortDirection::kDescending});
  REQUIRE(apps.size() == kApps);
  CHECK(apps.front()->name == "web_browser.exe");
  CHECK(apps.back()->name == "notepad.exe");
}

TEST_CASE("Rows sort by name, A to Z") {
  const data::DummyProcessTable table;
  const std::vector<const data::ProcessRow*> apps = RowsInGroup(
      table.rows(), data::ProcessGroup::kApp,
      {.column = ProcessColumn::kName, .direction = SortDirection::kAscending});
  REQUIRE(apps.size() == kApps);
  CHECK(apps.front()->name == "csopesy_shell.exe");
  CHECK(apps.back()->name == "web_browser.exe");
}

TEST_CASE("End task removes the selected process and clears the selection") {
  data::DummyProcessTable table;
  TaskManager task_manager(table);
  task_manager.set_selected_process("notepad.exe");
  task_manager.EndSelectedTask();
  CHECK(task_manager.selected_process().empty());
  CHECK(table.CountInGroup(data::ProcessGroup::kApp) == kApps - 1);
}

TEST_CASE("End task does nothing without a selection") {
  data::DummyProcessTable table;
  TaskManager task_manager(table);
  task_manager.EndSelectedTask();
  CHECK(table.rows().size() == kApps + kBackground);
}

}  // namespace
}  // namespace csopesy::apps
