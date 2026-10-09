#ifndef CSOPESY_SRC_APPS_TASK_MANAGER_H_
#define CSOPESY_SRC_APPS_TASK_MANAGER_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"

#include "apps/app_window.h"
#include "apps/sort_direction.h"
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

enum class ProcessColumn : std::uint8_t { kName, kStatus, kCpu, kMemory };

// How the process rows are ordered. With no column, rows keep the table's
// order.
struct ProcessSort {
  std::optional<ProcessColumn> column;
  SortDirection direction = SortDirection::kAscending;
};

// The sort after a click on a column header: the sorted column flips
// direction; another column starts heaviest first for CPU and Memory (as
// Windows does) and A to Z for Name and Status.
[[nodiscard]] ProcessSort ToggleSort(const ProcessSort& current,
                                     ProcessColumn clicked);

// The rows of one group in display order. Rows that tie keep name order.
[[nodiscard]] std::vector<const data::ProcessRow*> RowsInGroup(
    std::span<const data::ProcessRow> rows, data::ProcessGroup group,
    const ProcessSort& sort);

// The Windows-style Task Manager, Processes view: a tab strip, then a table
// of the dummy processes grouped into apps and background processes, with
// the CPU and memory totals above the column names.
class TaskManager : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{720.0F, 480.0F};

  // The table is borrowed and must outlive the window. "End task" removes
  // rows from it.
  explicit TaskManager(data::DummyProcessTable& table)
      : AppWindow("Task Manager", kDefaultSize), table_(&table) {}

  [[nodiscard]] std::string_view selected_process() const {
    return selected_process_;
  }
  void set_selected_process(std::string_view name) { selected_process_ = name; }
  [[nodiscard]] const ProcessSort& sort() const { return sort_; }

  // Ends the selected process, as the "End task" button does.
  void EndSelectedTask();

 protected:
  void Draw() override;

 private:
  void DrawToolbar();
  void DrawProcessTable();
  void DrawHeaderRow();
  void DrawGroup(data::ProcessGroup group);

  data::DummyProcessTable* table_;
  std::string_view selected_process_;
  ProcessSort sort_;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_TASK_MANAGER_H_
