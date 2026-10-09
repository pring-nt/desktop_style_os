#include "data/dummy_process_table.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
// doctest prints std::string_view operands with operator<<.
#include <ostream>  // IWYU pragma: keep
#include <set>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

namespace csopesy::data {
namespace {

constexpr std::size_t kExpectedRows = 20;
constexpr std::size_t kExpectedApps = 6;
constexpr double kHeavyCpu = 70.0;
constexpr double kHeavyMemory = 6000.0;
constexpr float kHalf = 0.5F;
constexpr int kManyTicks = 50;
// Values are rounded to 0.1 after jittering.
constexpr double kRounding = 0.05;

bool SameValues(const ProcessRow& first, const ProcessRow& second) {
  return first.name == second.name && first.cpu_percent == second.cpu_percent &&
         first.memory_mb == second.memory_mb;
}

// Whether a jittered row is within the jitter range of its starting values,
// and unchanged if it is suspended.
bool StaysNearBase(const ProcessRow& start, const ProcessRow& now) {
  if (start.status == ProcessStatus::kSuspended) {
    return SameValues(start, now);
  }
  const double cpu_limit =
      (start.cpu_percent * kCpuJitterShare) + kCpuJitterFloor + kRounding;
  const double memory_limit =
      (start.memory_mb * kMemoryJitterShare) + kRounding;
  return now.cpu_percent >= 0.0 &&
         std::abs(now.cpu_percent - start.cpu_percent) <= cpu_limit &&
         std::abs(now.memory_mb - start.memory_mb) <= memory_limit;
}

TEST_CASE("DummyProcessTable has a fixed list of 20 processes") {
  const DummyProcessTable table;
  CHECK(table.rows().size() == kExpectedRows);
  CHECK(table.CountInGroup(ProcessGroup::kApp) == kExpectedApps);
  CHECK(table.CountInGroup(ProcessGroup::kApp) +
            table.CountInGroup(ProcessGroup::kBackground) ==
        kExpectedRows);
}

TEST_CASE("DummyProcessTable rows have unique names and valid values") {
  const DummyProcessTable table;
  std::set<std::string_view> names;
  bool values_valid = true;
  for (const ProcessRow& row : table.rows()) {
    names.insert(row.name);
    values_valid = values_valid && row.cpu_percent >= 0.0 &&
                   row.cpu_percent <= kMaxPercent && row.memory_mb > 0.0;
  }
  CHECK(names.size() == table.rows().size());
  CHECK(values_valid);
}

TEST_CASE("DummyProcessTable totals equal the sums of the rows") {
  const DummyProcessTable table;
  double cpu = 0.0;
  double memory = 0.0;
  for (const ProcessRow& row : table.rows()) {
    cpu += row.cpu_percent;
    memory += row.memory_mb;
  }
  const ProcessTotals totals = table.totals();
  CHECK(totals.cpu_percent == doctest::Approx(cpu));
  CHECK(totals.memory_mb == doctest::Approx(memory));
  CHECK(totals.memory_percent ==
        doctest::Approx(memory / kSystemMemoryMb * kMaxPercent));
}

TEST_CASE("ComputeTotals caps CPU and memory at 100%") {
  constexpr std::array kHeavyRows{
      ProcessRow{
          .name = "a.exe",
          .group = ProcessGroup::kApp,
          .status = ProcessStatus::kRunning,
          .cpu_percent = kHeavyCpu,
          .memory_mb = kHeavyMemory,
      },
      ProcessRow{
          .name = "b.exe",
          .group = ProcessGroup::kApp,
          .status = ProcessStatus::kRunning,
          .cpu_percent = kHeavyCpu,
          .memory_mb = kHeavyMemory,
      },
  };
  const ProcessTotals totals = ComputeTotals(kHeavyRows);
  CHECK(totals.cpu_percent == kMaxPercent);
  CHECK(totals.memory_percent == kMaxPercent);
  CHECK(totals.memory_mb == doctest::Approx(kHeavyMemory + kHeavyMemory));
}

TEST_CASE("ComputeTotals of no rows is zero") {
  const ProcessTotals totals = ComputeTotals({});
  CHECK(totals.cpu_percent == 0.0);
  CHECK(totals.memory_mb == 0.0);
}

TEST_CASE("Advance jitters only once a full interval has passed") {
  DummyProcessTable table;
  const std::vector<ProcessRow> before(table.rows().begin(),
                                       table.rows().end());
  table.Advance(kJitterInterval * kHalf);
  CHECK(std::ranges::equal(table.rows(), before, SameValues));
  table.Advance(kJitterInterval * kHalf);
  CHECK_FALSE(std::ranges::equal(table.rows(), before, SameValues));
}

TEST_CASE("Jitter stays near the base values and leaves suspended rows") {
  const DummyProcessTable base;
  DummyProcessTable table;
  for (int tick = 0; tick < kManyTicks; ++tick) {
    table.Jitter();
  }
  CHECK(std::ranges::equal(base.rows(), table.rows(), StaysNearBase));
}

TEST_CASE("Two tables jitter the same way from the fixed seed") {
  DummyProcessTable first;
  DummyProcessTable second;
  first.Jitter();
  second.Jitter();
  CHECK(std::ranges::equal(first.rows(), second.rows(), SameValues));
}

TEST_CASE("EndProcess removes the row and keeps the rest jittering") {
  DummyProcessTable table;
  CHECK(table.EndProcess("notepad.exe"));
  CHECK(table.rows().size() == kExpectedRows - 1);
  CHECK(table.CountInGroup(ProcessGroup::kApp) == kExpectedApps - 1);
  CHECK_FALSE(table.EndProcess("notepad.exe"));
  table.Jitter();
  CHECK(std::ranges::none_of(table.rows(), [](const ProcessRow& row) {
    return row.name == "notepad.exe";
  }));
}

TEST_CASE("StatusLabel names each status") {
  CHECK(StatusLabel(ProcessStatus::kRunning) == "Running");
  CHECK(StatusLabel(ProcessStatus::kSuspended) == "Suspended");
}

}  // namespace
}  // namespace csopesy::data
