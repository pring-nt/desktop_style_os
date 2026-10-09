#ifndef CSOPESY_SRC_APPS_TASK_MANAGER_H_
#define CSOPESY_SRC_APPS_TASK_MANAGER_H_

#include <cstddef>
#include <string>
#include <string_view>

#include "imgui.h"

#include "apps/app_window.h"
#include "data/dummy_process_table.h"

namespace csopesy::apps {

// A process at this CPU % or memory gets the darkest shading.
inline constexpr double kCpuFullHeatPercent = 10.0;
inline constexpr double kMemoryFullHeatMb = 500.0;

// How heavy `value` is, from 0 (idle) to 1 (at or above `full_scale`).
[[nodiscard]] float UsageHeat(double value, double full_scale);
// The CPU and Memory cell shading for a heat: pale yellow at 0, deep amber
// at 1, as in the Windows Task Manager.
[[nodiscard]] ImVec4 HeatColor(float heat);

// "3.4%": a process's CPU usage as the CPU column shows it.
[[nodiscard]] std::string FormatCpu(double percent);
// "128.6 MB": a process's memory as the Memory column shows it.
[[nodiscard]] std::string FormatMemory(double megabytes);
// "22%": a column total, rounded, as shown above the column name.
[[nodiscard]] std::string FormatTotal(double percent);
// "Apps (6)": a group header row.
[[nodiscard]] std::string FormatGroupHeader(data::ProcessGroup group,
                                            std::size_t count);

// The Windows-style Task Manager, Processes view: a tab strip, then a table
// of the dummy processes grouped into apps and background processes, with
// the CPU and memory totals above the column names.
class TaskManager : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{720.0F, 480.0F};

  // The table is borrowed and must outlive the window.
  explicit TaskManager(const data::DummyProcessTable& table)
      : AppWindow("Task Manager", kDefaultSize), table_(&table) {}

  [[nodiscard]] std::string_view selected_process() const {
    return selected_process_;
  }

 protected:
  void Draw() override;

 private:
  void DrawProcessTable();
  void DrawGroup(data::ProcessGroup group);

  const data::DummyProcessTable* table_;
  std::string_view selected_process_;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_TASK_MANAGER_H_
