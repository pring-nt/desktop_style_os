#include "data/settings.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <string>
#include <string_view>

#include "data/text.h"

namespace csopesy::data {

namespace {

constexpr std::string_view kWallpaperKey = "wallpaper";
constexpr char kSeparator = '=';
constexpr std::string_view kHeader =
    "# CSOPESY OS settings, written by the app.\n";

bool IsComment(std::string_view line) {
  return line.starts_with('#') || line.starts_with(';');
}

}  // namespace

Settings ParseSettings(std::string_view text) {
  Settings settings;
  for (const std::string_view line : SplitLines(text)) {
    const std::size_t separator = line.find(kSeparator);
    if (line.empty() || IsComment(line) ||
        separator == std::string_view::npos) {
      continue;
    }
    const std::string_view key = TrimLine(line.substr(0, separator));
    const std::string_view value = TrimLine(line.substr(separator + 1));
    if (key == kWallpaperKey && !value.empty()) {
      settings.wallpaper = std::string(value);
    }
  }
  return settings;
}

std::string FormatSettings(const Settings& settings) {
  std::string text(kHeader);
  if (settings.wallpaper) {
    text += kWallpaperKey;
    text += kSeparator;
    text += *settings.wallpaper;
    text += '\n';
  }
  return text;
}

Settings LoadSettings(const std::filesystem::path& path) {
  const std::optional<std::string> text = ReadTextFile(path);
  return text ? ParseSettings(*text) : Settings{};
}

bool SaveSettings(const std::filesystem::path& path, const Settings& settings) {
  std::ofstream file(path, std::ios::binary);
  if (!file) {
    return false;
  }
  file << FormatSettings(settings);
  return static_cast<bool>(file);
}

}  // namespace csopesy::data
