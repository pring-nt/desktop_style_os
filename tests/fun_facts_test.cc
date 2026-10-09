#include "data/fun_facts.h"

#include <cstdint>
#include <filesystem>
#include <ostream>  // IWYU pragma: keep (doctest prints std::string_view)
#include <string>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

namespace csopesy::data {
namespace {

constexpr std::uint64_t kSecondFact = 1;
constexpr std::uint64_t kWrapsToFirst = 3;
constexpr std::uint64_t kAnySeed = 42;

TEST_CASE("Fun facts are one per line, skipping blanks and comments") {
  const std::vector<std::string> facts =
      ParseFunFacts("# header\r\n\r\n  First fact.  \r\nSecond fact.\n\n");
  REQUIRE(facts.size() == 2);
  CHECK(facts.front() == "First fact.");
  CHECK(facts.back() == "Second fact.");
}

TEST_CASE("A facts file without a trailing newline keeps its last line") {
  CHECK(ParseFunFacts("Only fact").size() == 1);
  CHECK(ParseFunFacts("").empty());
}

TEST_CASE("The fact is picked by the seed, wrapping around") {
  const std::vector<std::string> facts{"a", "b", "c"};
  CHECK(PickFunFact(facts, kSecondFact) == "b");
  CHECK(PickFunFact(facts, kWrapsToFirst) == "a");
}

TEST_CASE("No facts falls back to the built-in fact") {
  CHECK(PickFunFact({}, kAnySeed) == kFallbackFunFact);
  CHECK(LoadFunFacts("no/such/file.txt").empty());
}

TEST_CASE("The shipped facts file has facts") {
  const std::filesystem::path path =
      std::filesystem::path(CSOPESY_SOURCE_DIR) / kFunFactsPath;
  CHECK_FALSE(LoadFunFacts(path).empty());
}

}  // namespace
}  // namespace csopesy::data
