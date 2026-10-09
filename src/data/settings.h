#ifndef CSOPESY_SRC_DATA_SETTINGS_H_
#define CSOPESY_SRC_DATA_SETTINGS_H_

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace csopesy::data {

// Saved next to the executable, so each build keeps its own choices.
inline constexpr std::string_view kSettingsFileName = "settings.ini";

// What the user chose and the app remembers between launches.
struct Settings {
  // A file name in the wallpaper folder, or kGradientWallpaper. Unset means
  // nothing was saved, so the default is used.
  std::optional<std::string> wallpaper;
};

// Reads "key=value" lines. Blank lines, lines starting with '#' or ';', and
// unknown keys are ignored.
[[nodiscard]] Settings ParseSettings(std::string_view text);
[[nodiscard]] std::string FormatSettings(const Settings& settings);

// Empty settings if the file is missing or unreadable.
[[nodiscard]] Settings LoadSettings(const std::filesystem::path& path);
// Returns false if the file couldn't be written.
bool SaveSettings(const std::filesystem::path& path, const Settings& settings);

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_SETTINGS_H_
