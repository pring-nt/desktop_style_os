#include "data/wallpaper_catalog.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace csopesy::data {

namespace {

constexpr std::array<std::string_view, 3> kImageExtensions{
    ".jpg",
    ".jpeg",
    ".png",
};

std::string ToLower(std::string_view text) {
  std::string lower(text);
  std::ranges::transform(lower, lower.begin(), [](unsigned char letter) {
    return static_cast<char>(std::tolower(letter));
  });
  return lower;
}

bool Contains(std::span<const std::string> names, std::string_view name) {
  return std::ranges::find(names, name) != names.end();
}

}  // namespace

bool IsWallpaperFile(std::string_view file_name) {
  const std::string lower = ToLower(file_name);
  return std::ranges::any_of(
      kImageExtensions, [&lower](std::string_view extension) {
        return lower.size() > extension.size() && lower.ends_with(extension);
      });
}

std::vector<std::string> ListWallpapers(
    const std::filesystem::path& directory) {
  std::vector<std::string> names;
  std::error_code error;
  std::filesystem::directory_iterator it(directory, error);
  for (; !error && it != std::filesystem::directory_iterator();
       it.increment(error)) {
    const std::string name = it->path().filename().string();
    if (it->is_regular_file(error) && IsWallpaperFile(name)) {
      names.push_back(name);
    }
  }
  std::ranges::sort(names);
  return names;
}

std::string WallpaperLabel(std::string_view file_name) {
  std::string label = std::filesystem::path(file_name).stem().string();
  std::ranges::replace(label, '_', ' ');
  return label;
}

std::string ResolveWallpaper(const std::optional<std::string>& saved,
                             std::span<const std::string> available) {
  if (saved && (*saved == kGradientWallpaper || Contains(available, *saved))) {
    return *saved;
  }
  if (Contains(available, kDefaultWallpaper)) {
    return std::string(kDefaultWallpaper);
  }
  return std::string(kGradientWallpaper);
}

}  // namespace csopesy::data
