#include "data/terminal_commands.h"

#include <cstddef>
#include <optional>
// doctest prints std::string_view operands with operator<<.
#include <ostream>  // IWYU pragma: keep
#include <string>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

#include "data/dummy_process_table.h"

namespace csopesy::data {
namespace {

constexpr std::size_t kHelpLines = 10;
// A header row, one line per process, then a totals line.
constexpr std::size_t kPsExtraLines = 2;

CommandResult Run(std::string_view input) {
  static const DummyProcessTable kTable;
  return RunCommand(input, {
                               .date = "Thursday, Oct 08, 2026",
                               .time = "10:51 PM",
                               .processes = &kTable,
                           });
}

TEST_CASE("ver prints the OS version") {
  const CommandResult result = Run("ver");
  REQUIRE(result.lines.size() == 1);
  CHECK(result.lines.front() == "CSOPESY OS v1.0");
  CHECK(result.effect == CommandEffect::kNone);
}

TEST_CASE("help lists every command") {
  const CommandResult result = Run("help");
  REQUIRE(result.lines.size() == kHelpLines);
  CHECK(result.lines.front() == "Available commands:");
  CHECK(result.lines.back() == "  exit     Close the Terminal");
}

TEST_CASE("date and time print the clock's values") {
  CHECK(
      Run("date").lines ==
      std::vector<std::string>{"The current date is: Thursday, Oct 08, 2026"});
  CHECK(Run("time").lines ==
        std::vector<std::string>{"The current time is: 10:51 PM"});
}

TEST_CASE("echo prints its text") {
  CHECK(Run("echo hello  world").lines ==
        std::vector<std::string>{"hello  world"});
  CHECK(Run("echo").lines == std::vector<std::string>{"ECHO is on."});
}

TEST_CASE("whoami prints the fake user") {
  CHECK(Run("whoami").lines == std::vector<std::string>{R"(csopesy\user)"});
}

TEST_CASE("ps lists every dummy process") {
  const DummyProcessTable table;
  const CommandResult result = Run("ps");
  REQUIRE(result.lines.size() == table.rows().size() + kPsExtraLines);
  CHECK(result.lines.front().starts_with("Name"));
  CHECK(result.lines.at(1).starts_with("csopesy_shell.exe"));
  CHECK(result.lines.back().starts_with("20 processes"));
}

TEST_CASE("cls clears the screen and exit closes the window") {
  const CommandResult cls = Run("cls");
  CHECK(cls.lines.empty());
  CHECK(cls.effect == CommandEffect::kClearScreen);
  const CommandResult exit = Run("exit");
  CHECK(exit.lines.empty());
  CHECK(exit.effect == CommandEffect::kCloseWindow);
}

TEST_CASE("Unknown commands are reported") {
  CHECK(Run("format c:").lines ==
        std::vector<std::string>{"'format' is not recognized as a command."});
}

TEST_CASE("Commands ignore case and surrounding spaces") {
  CHECK(Run("  VER  ").lines == std::vector<std::string>{"CSOPESY OS v1.0"});
  CHECK(Run("   ").lines.empty());
}

TEST_CASE("The banner starts with the OS name and version") {
  REQUIRE_FALSE(BannerLines().empty());
  CHECK(BannerLines().front() == "CSOPESY OS v1.0");
}

TEST_CASE("CommandHistory browses from newest to oldest and back") {
  CommandHistory history;
  history.Add("ver");
  history.Add("help");
  history.Add("ps");

  CHECK(history.Previous() == "ps");
  CHECK(history.Previous() == "help");
  CHECK(history.Previous() == "ver");
  CHECK(history.Previous() == "ver");
  CHECK(history.Next() == "help");
  CHECK(history.Next() == "ps");
  CHECK(history.Next() == "");
  CHECK(history.Next() == std::nullopt);
}

TEST_CASE("CommandHistory skips blanks and repeats") {
  CommandHistory history;
  CHECK(history.Previous() == std::nullopt);
  history.Add("ver");
  history.Add("ver");
  history.Add("  ");
  CHECK(history.size() == 1);
}

TEST_CASE("Adding a command restarts history browsing at the newest") {
  CommandHistory history;
  history.Add("ver");
  history.Add("help");
  CHECK(history.Previous() == "help");
  CHECK(history.Previous() == "ver");
  history.Add("ps");
  CHECK(history.Previous() == "ps");
}

}  // namespace
}  // namespace csopesy::data
