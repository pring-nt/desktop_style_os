#include "core/clock.h"

#include <chrono>
#include <ctime>

#include <doctest/doctest.h>

namespace csopesy::core {
namespace {

using std::chrono::October;
using std::chrono_literals::operator""d;
using std::chrono_literals::operator""h;
using std::chrono_literals::operator""min;
using std::chrono_literals::operator""y;

constexpr int kTmYearBase = 1900;

// Builds the calendar fields that ToLocalTime() would produce for this local
// date and time, so formatting tests never depend on the machine's time zone.
std::tm MakeLocalTime(std::chrono::year_month_day date, std::chrono::hours hour,
                      std::chrono::minutes minute) {
  std::tm local_time{};
  local_time.tm_year = static_cast<int>(date.year()) - kTmYearBase;
  local_time.tm_mon = static_cast<int>(static_cast<unsigned>(date.month())) - 1;
  local_time.tm_mday = static_cast<int>(static_cast<unsigned>(date.day()));
  local_time.tm_wday = static_cast<int>(
      std::chrono::weekday(std::chrono::sys_days(date)).c_encoding());
  local_time.tm_hour = static_cast<int>(hour.count());
  local_time.tm_min = static_cast<int>(minute.count());
  return local_time;
}

TEST_CASE("FormatDateTime formats a fixed time point") {
  CHECK(FormatDateTime(MakeLocalTime(2026y / October / 8d, 19h, 43min)) ==
        "Thursday, Oct 08, 2026 | 07:43 PM");
}

TEST_CASE("FormatDateTime shows midnight as 12 AM") {
  CHECK(FormatDateTime(MakeLocalTime(2026y / October / 9d, 0h, 5min)) ==
        "Friday, Oct 09, 2026 | 12:05 AM");
}

TEST_CASE("FormatDateTime shows noon as 12 PM") {
  CHECK(FormatDateTime(MakeLocalTime(2026y / October / 9d, 12h, 0min)) ==
        "Friday, Oct 09, 2026 | 12:00 PM");
}

TEST_CASE("Clock reads the injected time source") {
  const auto fixed_time =
      std::chrono::sys_days(2026y / October / 8d) + 19h + 43min;
  const Clock clock([fixed_time] { return fixed_time; });

  CHECK(clock.Now() == fixed_time);
  CHECK(clock.FormattedNow() == FormatDateTime(ToLocalTime(fixed_time)));
}

}  // namespace
}  // namespace csopesy::core
