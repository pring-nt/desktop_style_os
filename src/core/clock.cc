#include "core/clock.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <utility>

namespace csopesy::core {

namespace {

constexpr const char* kDateTimeFormat = "%A, %b %d, %Y | %I:%M %p";

}  // namespace

std::tm ToLocalTime(std::chrono::system_clock::time_point time) {
  const std::time_t seconds = std::chrono::system_clock::to_time_t(time);
  // Left zeroed if the conversion fails, which only happens for time points
  // far outside any date the desktop can show.
  std::tm local_time{};
#ifdef _WIN32
  localtime_s(&local_time, &seconds);
#else
  localtime_r(&seconds, &local_time);
#endif
  return local_time;
}

std::string FormatDateTime(const std::tm& local_time) {
  std::ostringstream stream;
  stream.imbue(std::locale::classic());
  stream << std::put_time(&local_time, kDateTimeFormat);
  return stream.str();
}

Clock::Clock() : Clock(std::chrono::system_clock::now) {}

Clock::Clock(TimeSource time_source) : time_source_(std::move(time_source)) {}

std::chrono::system_clock::time_point Clock::Now() const {
  return time_source_();
}

std::string Clock::FormattedNow() const {
  return FormatDateTime(ToLocalTime(Now()));
}

}  // namespace csopesy::core
