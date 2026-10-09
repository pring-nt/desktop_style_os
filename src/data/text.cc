#include "data/text.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace csopesy::data {

namespace {

constexpr std::string_view kLineWhitespace = " \t\r";

}  // namespace

std::string_view TrimLine(std::string_view text) {
  const std::size_t first = text.find_first_not_of(kLineWhitespace);
  if (first == std::string_view::npos) {
    return {};
  }
  const std::size_t last = text.find_last_not_of(kLineWhitespace);
  return text.substr(first, last - first + 1);
}

std::vector<std::string_view> SplitLines(std::string_view text) {
  std::vector<std::string_view> lines;
  while (!text.empty()) {
    const std::size_t end = text.find('\n');
    lines.push_back(TrimLine(text.substr(0, end)));
    text = end == std::string_view::npos ? std::string_view{}
                                         : text.substr(end + 1);
  }
  return lines;
}

std::optional<std::string> ReadTextFile(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return std::nullopt;
  }
  return std::string{std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>()};
}

}  // namespace csopesy::data
