#include "data/fun_facts.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "data/text.h"

namespace csopesy::data {

namespace {

constexpr char kCommentMarker = '#';

}  // namespace

std::vector<std::string> ParseFunFacts(std::string_view text) {
  std::vector<std::string> facts;
  for (const std::string_view line : SplitLines(text)) {
    if (!line.empty() && line.front() != kCommentMarker) {
      facts.emplace_back(line);
    }
  }
  return facts;
}

std::vector<std::string> LoadFunFacts(const std::filesystem::path& path) {
  const std::optional<std::string> text = ReadTextFile(path);
  return text ? ParseFunFacts(*text) : std::vector<std::string>{};
}

std::string PickFunFact(std::span<const std::string> facts,
                        std::uint64_t seed) {
  if (facts.empty()) {
    return std::string(kFallbackFunFact);
  }
  return facts.subspan(static_cast<std::size_t>(seed % facts.size())).front();
}

}  // namespace csopesy::data
