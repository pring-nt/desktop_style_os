#ifndef CSOPESY_SRC_DATA_DUMMY_PROCESS_TABLE_H_
#define CSOPESY_SRC_DATA_DUMMY_PROCESS_TABLE_H_

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace csopesy::data {

// The fake machine's installed memory, used for the Memory % total.
inline constexpr double kSystemMemoryMb = 8192.0;
inline constexpr double kMaxPercent = 100.0;

// Which Task Manager group a process is listed under.
enum class ProcessGroup : std::uint8_t { kApp, kBackground };

enum class ProcessStatus : std::uint8_t { kRunning, kSuspended };

struct ProcessRow {
  std::string_view name;
  ProcessGroup group;
  ProcessStatus status;
  double cpu_percent;
  double memory_mb;
};

struct ProcessTotals {
  // Sum of every row's CPU, capped at 100%.
  double cpu_percent;
  // Sum of every row's memory.
  double memory_mb;
  // memory_mb as a share of kSystemMemoryMb, capped at 100%.
  double memory_percent;
};

// "Running" or "Suspended", as the Status column shows it.
[[nodiscard]] std::string_view StatusLabel(ProcessStatus status);

// Totals for the Task Manager's column headers.
[[nodiscard]] ProcessTotals ComputeTotals(std::span<const ProcessRow> rows);

// The fixed list of fake processes shown by the Task Manager and the
// Terminal's `ps`. Defined in one place so every run shows the same values.
class DummyProcessTable {
 public:
  DummyProcessTable();

  [[nodiscard]] std::span<const ProcessRow> rows() const { return rows_; }
  [[nodiscard]] ProcessTotals totals() const { return ComputeTotals(rows_); }
  [[nodiscard]] std::size_t CountInGroup(ProcessGroup group) const;

 private:
  std::span<const ProcessRow> rows_;
};

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_DUMMY_PROCESS_TABLE_H_
