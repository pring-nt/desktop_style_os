#ifndef CSOPESY_SRC_DATA_TEXT_H_
#define CSOPESY_SRC_DATA_TEXT_H_

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace csopesy::data {

// `text` without leading and trailing spaces, tabs and '\r' (so Windows line
// endings are dropped too).
[[nodiscard]] std::string_view TrimLine(std::string_view text);

// The lines of `text`, split at '\n' and trimmed. A trailing newline does not
// add an empty last line.
[[nodiscard]] std::vector<std::string_view> SplitLines(std::string_view text);

// The whole file as text, or nullopt if it can't be read.
[[nodiscard]] std::optional<std::string> ReadTextFile(
    const std::filesystem::path& path);

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_TEXT_H_
