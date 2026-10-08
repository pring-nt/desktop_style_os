#ifndef CSOPESY_SRC_DATA_TERMINAL_COMMANDS_H_
#define CSOPESY_SRC_DATA_TERMINAL_COMMANDS_H_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "data/dummy_process_table.h"

namespace csopesy::data {

inline constexpr std::string_view kTerminalPrompt = R"(C:\CSOPESY>)";
inline constexpr std::string_view kOsVersion = "CSOPESY OS v1.0";

// What a command needs from outside the terminal. Date and time arrive
// already formatted so the commands stay independent of the clock.
struct CommandContext {
  std::string_view date;
  std::string_view time;
  const DummyProcessTable* processes;
};

// What the terminal window must do besides printing the output lines.
enum class CommandEffect : std::uint8_t { kNone, kClearScreen, kCloseWindow };

struct CommandResult {
  std::vector<std::string> lines;
  CommandEffect effect = CommandEffect::kNone;
};

// The lines printed when the terminal opens.
[[nodiscard]] std::vector<std::string> BannerLines();

// Runs one line of input. Command names are case-insensitive, as in cmd.exe;
// blank input prints nothing.
[[nodiscard]] CommandResult RunCommand(std::string_view input,
                                       const CommandContext& context);

// Previously entered commands, browsed with the Up and Down arrows.
class CommandHistory {
 public:
  // Remembers a command and stops browsing. Blank input and a repeat of the
  // newest command are not stored.
  void Add(std::string_view command);

  // Up arrow: the next older command (stays on the oldest), or nullopt when
  // the history is empty.
  [[nodiscard]] std::optional<std::string_view> Previous();
  // Down arrow: the next newer command, an empty string after the newest
  // (clearing the input), or nullopt when not browsing.
  [[nodiscard]] std::optional<std::string_view> Next();

  [[nodiscard]] std::size_t size() const { return commands_.size(); }

 private:
  std::vector<std::string> commands_;
  // commands_.size() means "not browsing".
  std::size_t position_ = 0;
};

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_TERMINAL_COMMANDS_H_
