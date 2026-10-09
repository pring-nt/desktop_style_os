#include "data/fun_facts.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace csopesy::data {

namespace {

constexpr std::string_view kWhitespace = " \t\r";
constexpr char kCommentMarker = '#';

std::string_view Trim(std::string_view text) {
  const std::size_t first = text.find_first_not_of(kWhitespace);
  if (first == std::string_view::npos) {
    return {};
  }
  const std::size_t last = text.find_last_not_of(kWhitespace);
  return text.substr(first, last - first + 1);
}

}  // namespace

std::vector<std::string> ParseFunFacts(std::string_view text) {
  std::vector<std::string> facts;
  while (!text.empty()) {
    const std::size_t end = text.find('\n');
    const std::string_view line = Trim(text.substr(0, end));
    if (!line.empty() && line.front() != kCommentMarker) {
      facts.emplace_back(line);
    }
    text = end == std::string_view::npos ? std::string_view{}
                                         : text.substr(end + 1);
  }
  return facts;
}

std::vector<std::string> LoadFunFacts(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return {};
  }
  const std::string text{std::istreambuf_iterator<char>(file),
                         std::istreambuf_iterator<char>()};
  return ParseFunFacts(text);
}

std::string PickFunFact(std::span<const std::string> facts,
                        std::uint64_t seed) {
  if (facts.empty()) {
    return std::string(kFallbackFunFact);
  }
  return facts.subspan(static_cast<std::size_t>(seed % facts.size())).front();
}

}  // namespace csopesy::data
