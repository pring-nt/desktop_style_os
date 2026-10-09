#ifndef CSOPESY_SRC_DATA_FUN_FACTS_H_
#define CSOPESY_SRC_DATA_FUN_FACTS_H_

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace csopesy::data {

// Where the BIOS screen's fun facts live, relative to the executable.
inline constexpr std::string_view kFunFactsPath = "assets/fun_facts.txt";

// Shown when the facts file is missing or has no facts.
inline constexpr std::string_view kFallbackFunFact =
    "The first computer bug was a real moth, found in the Harvard Mark II in "
    "1947.";

// One fact per line. Blank lines and lines starting with '#' are skipped, and
// surrounding spaces (and a Windows '\r') are trimmed.
[[nodiscard]] std::vector<std::string> ParseFunFacts(std::string_view text);

// Reads and parses a facts file. Empty if the file can't be read.
[[nodiscard]] std::vector<std::string> LoadFunFacts(
    const std::filesystem::path& path);

// The fact for this boot: facts[seed % size], or kFallbackFunFact if there
// are none.
[[nodiscard]] std::string PickFunFact(std::span<const std::string> facts,
                                      std::uint64_t seed);

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_FUN_FACTS_H_
