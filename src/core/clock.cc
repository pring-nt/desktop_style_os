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

constexpr const char* kDateFormat = "%A, %b %d, %Y";
constexpr const char* kTimeFormat = "%I:%M %p";

std::string FormatWith(const std::tm& local_time, const char* format) {
  std::ostringstream stream;
  stream.imbue(std::locale::classic());
  stream << std::put_time(&local_time, format);
  return stream.str();
}

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

std::string FormatDate(const std::tm& local_time) {
  return FormatWith(local_time, kDateFormat);
}

std::string FormatTime(const std::tm& local_time) {
  return FormatWith(local_time, kTimeFormat);
}

std::string FormatDateTime(const std::tm& local_time) {
  return FormatDate(local_time) + " | " + FormatTime(local_time);
}

Clock::Clock() : Clock(std::chrono::system_clock::now) {}

Clock::Clock(TimeSource time_source) : time_source_(std::move(time_source)) {}

std::chrono::system_clock::time_point Clock::Now() const {
  return time_source_();
}

std::string Clock::FormattedNow() const {
  return FormatDateTime(ToLocalTime(Now()));
}

std::string Clock::FormattedDate() const {
  return FormatDate(ToLocalTime(Now()));
}

std::string Clock::FormattedTime() const {
  return FormatTime(ToLocalTime(Now()));
}

}  // namespace csopesy::core
