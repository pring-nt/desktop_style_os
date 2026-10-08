#include "data/dummy_process_table.h"

#include <array>
#include <cstddef>
// doctest prints std::string_view operands with operator<<.
#include <ostream>  // IWYU pragma: keep
#include <set>
#include <string_view>

#include <doctest/doctest.h>

namespace csopesy::data {
namespace {

constexpr std::size_t kExpectedRows = 20;
constexpr std::size_t kExpectedApps = 6;
constexpr double kHeavyCpu = 70.0;
constexpr double kHeavyMemory = 6000.0;

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

TEST_CASE("StatusLabel names each status") {
  CHECK(StatusLabel(ProcessStatus::kRunning) == "Running");
  CHECK(StatusLabel(ProcessStatus::kSuspended) == "Suspended");
}

}  // namespace
}  // namespace csopesy::data
