#include "data/terminal_commands.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "data/dummy_process_table.h"

namespace csopesy::data {

namespace {

constexpr std::string_view kWhitespace = " \t";

std::string_view Trim(std::string_view text) {
  const std::size_t first = text.find_first_not_of(kWhitespace);
  if (first == std::string_view::npos) {
    return {};
  }
  const std::size_t last = text.find_last_not_of(kWhitespace);
  return text.substr(first, last - first + 1);
}

std::string ToLower(std::string_view text) {
  std::string lower(text);
  std::ranges::transform(lower, lower.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return lower;
}

CommandResult PrintLine(std::string line) {
  CommandResult result;
  result.lines.push_back(std::move(line));
  return result;
}

std::vector<std::string> HelpLines() {
  return {
      "Available commands:",
      "  help     Show this list",
      "  ver      Show the OS version",
      "  date     Show today's date",
      "  time     Show the current time",
      "  echo     Print the text after it",
      "  ps       List running processes",
      "  whoami   Show the current user",
      "  cls      Clear the screen",
      "  exit     Close the Terminal",
  };
}

struct ProcessCells {
  std::string_view name;
  std::string_view status;
  std::string_view cpu;
  std::string_view memory;
};

std::string ProcessLine(const ProcessCells& cells) {
  return std::format("{:<24}{:<11}{:>7}{:>11}", cells.name, cells.status,
                     cells.cpu, cells.memory);
}

std::vector<std::string> ProcessLines(const DummyProcessTable& table) {
  std::vector<std::string> lines;
  lines.push_back(ProcessLine({
      .name = "Name",
      .status = "Status",
      .cpu = "CPU",
      .memory = "Memory",
  }));
  for (const ProcessRow& row : table.rows()) {
    const std::string cpu = std::format("{:.1f}%", row.cpu_percent);
    const std::string memory = std::format("{:.1f} MB", row.memory_mb);
    lines.push_back(ProcessLine({
        .name = row.name,
        .status = StatusLabel(row.status),
        .cpu = cpu,
        .memory = memory,
    }));
  }
  const ProcessTotals totals = table.totals();
  lines.push_back(std::format("{} processes, {:.1f}% CPU, {:.1f} MB memory",
                              table.rows().size(), totals.cpu_percent,
                              totals.memory_mb));
  return lines;
}

}  // namespace

std::vector<std::string> BannerLines() {
  return {
      std::string(kOsVersion),
      "(c) CSOPESY. All rights reserved.",
      "Type 'help' for a list of commands.",
      "",
  };
}

CommandResult RunCommand(std::string_view input,
                         const CommandContext& context) {
  const std::string_view line = Trim(input);
  if (line.empty()) {
    return {};
  }
  const std::size_t name_end = line.find_first_of(kWhitespace);
  const std::string_view name = line.substr(0, name_end);
  const std::string_view arguments = name_end == std::string_view::npos
                                         ? std::string_view{}
                                         : Trim(line.substr(name_end));
  const std::string command = ToLower(name);

  if (command == "help") {
    return {.lines = HelpLines()};
  }
  if (command == "ver") {
    return PrintLine(std::string(kOsVersion));
  }
  if (command == "date") {
    return PrintLine(std::format("The current date is: {}", context.date));
  }
  if (command == "time") {
    return PrintLine(std::format("The current time is: {}", context.time));
  }
  if (command == "echo") {
    return PrintLine(arguments.empty() ? std::string("ECHO is on.")
                                       : std::string(arguments));
  }
  if (command == "ps" && context.processes != nullptr) {
    return {.lines = ProcessLines(*context.processes)};
  }
  if (command == "whoami") {
    return PrintLine(R"(csopesy\user)");
  }
  if (command == "cls") {
    return {.lines = {}, .effect = CommandEffect::kClearScreen};
  }
  if (command == "exit") {
    return {.lines = {}, .effect = CommandEffect::kCloseWindow};
  }
  return PrintLine(std::format("'{}' is not recognized as a command.", name));
}

void CommandHistory::Add(std::string_view command) {
  const std::string_view trimmed = Trim(command);
  if (!trimmed.empty() && (commands_.empty() || commands_.back() != trimmed)) {
    commands_.emplace_back(trimmed);
  }
  position_ = commands_.size();
}

std::optional<std::string_view> CommandHistory::Previous() {
  if (commands_.empty()) {
    return std::nullopt;
  }
  if (position_ > 0) {
    --position_;
  }
  return commands_.at(position_);
}

std::optional<std::string_view> CommandHistory::Next() {
  if (position_ >= commands_.size()) {
    return std::nullopt;
  }
  ++position_;
  if (position_ == commands_.size()) {
    return std::string_view{};
  }
  return commands_.at(position_);
}

}  // namespace csopesy::data
