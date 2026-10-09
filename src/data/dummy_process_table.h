#ifndef CSOPESY_SRC_DATA_DUMMY_PROCESS_TABLE_H_
#define CSOPESY_SRC_DATA_DUMMY_PROCESS_TABLE_H_

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "data/random.h"

namespace csopesy::data {

// The fake machine's installed memory, used for the Memory % total.
inline constexpr double kSystemMemoryMb = 8192.0;
inline constexpr double kMaxPercent = 100.0;

// Live values: every kJitterInterval each running process's numbers move a
// little around their base values, from a fixed seed so runs repeat.
inline constexpr std::chrono::duration<float> kJitterInterval{1.0F};
inline constexpr std::uint64_t kJitterSeed = 12;
// CPU moves by up to this share of its base value, plus kCpuJitterFloor so
// idle processes flicker too.
inline constexpr double kCpuJitterShare = 0.3;
inline constexpr double kCpuJitterFloor = 0.2;
// Memory moves by up to this share of its base value.
inline constexpr double kMemoryJitterShare = 0.02;

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

// The fake processes shown by the Task Manager and the Terminal's `ps`.
// They start from a fixed list defined in one place; Advance() jitters the
// values from a fixed seed, and EndProcess() removes a row.
class DummyProcessTable {
 public:
  DummyProcessTable();

  [[nodiscard]] std::span<const ProcessRow> rows() const { return rows_; }
  [[nodiscard]] ProcessTotals totals() const { return ComputeTotals(rows_); }
  [[nodiscard]] std::size_t CountInGroup(ProcessGroup group) const;

  // Jitters the values once for every kJitterInterval of elapsed time.
  void Advance(std::chrono::duration<float> elapsed);
  // Moves each running process's CPU and memory a little around its base
  // value, rounded to the 0.1 the Task Manager shows so the header totals
  // still match the rows. Suspended processes stay at 0% CPU.
  void Jitter();
  // Removes the named process, as "End task" does. False if there is none.
  bool EndProcess(std::string_view name);

 private:
  // The fixed starting values, in the same order as rows_.
  std::vector<ProcessRow> base_;
  std::vector<ProcessRow> rows_;
  SplitMix64 random_{kJitterSeed};
  std::chrono::duration<float> since_jitter_{0.0F};
};

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_DUMMY_PROCESS_TABLE_H_
