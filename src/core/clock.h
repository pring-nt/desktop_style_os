#ifndef CSOPESY_SRC_CORE_CLOCK_H_
#define CSOPESY_SRC_CORE_CLOCK_H_

#include <chrono>
#include <ctime>
#include <functional>
#include <string>

namespace csopesy::core {

// Converts a time point to local calendar fields in the machine's time zone.
[[nodiscard]] std::tm ToLocalTime(std::chrono::system_clock::time_point time);

// Formats local calendar fields as "Thursday, Oct 08, 2026 | 07:43 PM".
// Always uses the classic "C" locale, so the text is the same on every
// machine.
[[nodiscard]] std::string FormatDateTime(const std::tm& local_time);

// Source of the current time for the desktop clock. The time source is
// injected so tests can use a fixed time point instead of the wall clock.
class Clock {
 public:
  using TimeSource = std::function<std::chrono::system_clock::time_point()>;

  // Reads std::chrono::system_clock::now().
  Clock();
  explicit Clock(TimeSource time_source);

  [[nodiscard]] std::chrono::system_clock::time_point Now() const;

  // The current local time, formatted by FormatDateTime().
  [[nodiscard]] std::string FormattedNow() const;

 private:
  TimeSource time_source_;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_CLOCK_H_
