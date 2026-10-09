#include "data/dummy_process_table.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

namespace csopesy::data {

namespace {

using enum ProcessGroup;
using enum ProcessStatus;

constexpr std::array kRows{
    ProcessRow{
        .name = "csopesy_shell.exe",
        .group = kApp,
        .status = kRunning,
        .cpu_percent = 3.4,
        .memory_mb = 128.6,
    },
    ProcessRow{
        .name = "file_explorer.exe",
        .group = kApp,
        .status = kRunning,
        .cpu_percent = 1.2,
        .memory_mb = 64.3,
    },
    ProcessRow{
        .name = "terminal.exe",
        .group = kApp,
        .status = kRunning,
        .cpu_percent = 0.8,
        .memory_mb = 22.5,
    },
    ProcessRow{
        .name = "task_manager.exe",
        .group = kApp,
        .status = kRunning,
        .cpu_percent = 2.1,
        .memory_mb = 38.9,
    },
    ProcessRow{
        .name = "notepad.exe",
        .group = kApp,
        .status = kRunning,
        .cpu_percent = 0.0,
        .memory_mb = 12.4,
    },
    ProcessRow{
        .name = "web_browser.exe",
        .group = kApp,
        .status = kRunning,
        .cpu_percent = 6.7,
        .memory_mb = 412.8,
    },
    ProcessRow{
        .name = "window_compositor.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 1.5,
        .memory_mb = 96.0,
    },
    ProcessRow{
        .name = "antivirus_scan.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 2.4,
        .memory_mb = 142.3,
    },
    ProcessRow{
        .name = "search_indexer.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 1.8,
        .memory_mb = 76.4,
    },
    ProcessRow{
        .name = "wallpaper_service.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.6,
        .memory_mb = 48.1,
    },
    ProcessRow{
        .name = "network_manager.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.5,
        .memory_mb = 15.9,
    },
    ProcessRow{
        .name = "audio_service.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.3,
        .memory_mb = 18.7,
    },
    ProcessRow{
        .name = "input_handler.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.2,
        .memory_mb = 6.1,
    },
    ProcessRow{
        .name = "clock_service.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.1,
        .memory_mb = 4.2,
    },
    ProcessRow{
        .name = "power_manager.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.1,
        .memory_mb = 5.6,
    },
    ProcessRow{
        .name = "font_cache.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.0,
        .memory_mb = 11.0,
    },
    ProcessRow{
        .name = "time_sync.exe",
        .group = kBackground,
        .status = kRunning,
        .cpu_percent = 0.0,
        .memory_mb = 3.4,
    },
    ProcessRow{
        .name = "bluetooth_service.exe",
        .group = kBackground,
        .status = kSuspended,
        .cpu_percent = 0.0,
        .memory_mb = 7.3,
    },
    ProcessRow{
        .name = "print_spooler.exe",
        .group = kBackground,
        .status = kSuspended,
        .cpu_percent = 0.0,
        .memory_mb = 9.8,
    },
    ProcessRow{
        .name = "update_service.exe",
        .group = kBackground,
        .status = kSuspended,
        .cpu_percent = 0.0,
        .memory_mb = 31.2,
    },
};

constexpr double kDisplayStepsPerUnit = 10.0;

// Rounds to the 0.1 step the Task Manager displays.
double RoundToDisplay(double value) {
  return std::round(value * kDisplayStepsPerUnit) / kDisplayStepsPerUnit;
}

}  // namespace

std::string_view StatusLabel(ProcessStatus status) {
  switch (status) {
    case kRunning:
      return "Running";
    case kSuspended:
      return "Suspended";
  }
  return "";
}

ProcessTotals ComputeTotals(std::span<const ProcessRow> rows) {
  double cpu = 0.0;
  double memory = 0.0;
  for (const ProcessRow& row : rows) {
    cpu += row.cpu_percent;
    memory += row.memory_mb;
  }
  return {
      .cpu_percent = std::min(cpu, kMaxPercent),
      .memory_mb = memory,
      .memory_percent =
          std::min(memory / kSystemMemoryMb * kMaxPercent, kMaxPercent),
  };
}

DummyProcessTable::DummyProcessTable()
    : base_(kRows.begin(), kRows.end()), rows_(base_) {}

std::size_t DummyProcessTable::CountInGroup(ProcessGroup group) const {
  return static_cast<std::size_t>(
      std::ranges::count(rows_, group, &ProcessRow::group));
}

void DummyProcessTable::Advance(std::chrono::duration<float> elapsed) {
  since_jitter_ += elapsed;
  while (since_jitter_ >= kJitterInterval) {
    since_jitter_ -= kJitterInterval;
    Jitter();
  }
}

void DummyProcessTable::Jitter() {
  for (std::size_t i = 0; i < rows_.size(); ++i) {
    const ProcessRow& base = base_.at(i);
    ProcessRow& row = rows_.at(i);
    if (row.status == kSuspended) {
      continue;
    }
    const double cpu_spread =
        (base.cpu_percent * kCpuJitterShare) + kCpuJitterFloor;
    row.cpu_percent = RoundToDisplay(
        std::clamp(base.cpu_percent + (random_.NextSigned() * cpu_spread), 0.0,
                   kMaxPercent));
    row.memory_mb = RoundToDisplay(
        base.memory_mb * (1.0 + (random_.NextSigned() * kMemoryJitterShare)));
  }
}

bool DummyProcessTable::EndProcess(std::string_view name) {
  const auto found = std::ranges::find(rows_, name, &ProcessRow::name);
  if (found == rows_.end()) {
    return false;
  }
  const auto index = std::distance(rows_.begin(), found);
  rows_.erase(found);
  base_.erase(std::next(base_.begin(), index));
  return true;
}

}  // namespace csopesy::data
