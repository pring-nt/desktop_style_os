#include "data/settings.h"

#include <filesystem>
#include <ostream>  // IWYU pragma: keep (doctest prints std::string_view)
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

#include "data/text.h"

namespace csopesy::data {
namespace {

TEST_CASE("Settings read the wallpaper and skip everything else") {
  const Settings settings = ParseSettings(
      "# comment\r\n; another\r\n\r\ntheme = dark\r\n  wallpaper = a b.png "
      "\r\n");
  CHECK(settings.wallpaper == "a b.png");
}

TEST_CASE("Settings without a wallpaper leave it unset") {
  CHECK_FALSE(ParseSettings("").wallpaper.has_value());
  CHECK_FALSE(ParseSettings("wallpaper=\nno separator").wallpaper.has_value());
}

TEST_CASE("Settings round-trip through text") {
  const Settings saved{.wallpaper = "frieren.jpg"};
  CHECK(ParseSettings(FormatSettings(saved)).wallpaper == saved.wallpaper);
}

TEST_CASE("Settings round-trip through a file") {
  const std::filesystem::path path =
      std::filesystem::temp_directory_path() / "csopesy_settings_test.ini";
  REQUIRE(SaveSettings(path, {.wallpaper = "gradient"}));
  CHECK(LoadSettings(path).wallpaper == "gradient");
  std::filesystem::remove(path);
  CHECK_FALSE(LoadSettings(path).wallpaper.has_value());
}

TEST_CASE("Lines are split and trimmed, without a trailing empty line") {
  const std::vector<std::string_view> lines = SplitLines(" a \r\n\tb\n");
  REQUIRE(lines.size() == 2);
  CHECK(lines.front() == "a");
  CHECK(lines.back() == "b");
  CHECK(TrimLine(" \t\r ").empty());
}

TEST_CASE("Reading a missing text file gives nothing") {
  CHECK_FALSE(ReadTextFile("no/such/file.txt").has_value());
}

}  // namespace
}  // namespace csopesy::data
