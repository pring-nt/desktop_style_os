#ifndef CSOPESY_SRC_DATA_WALLPAPER_CATALOG_H_
#define CSOPESY_SRC_DATA_WALLPAPER_CATALOG_H_

#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace csopesy::data {

// The wallpaper folder, relative to the executable.
inline constexpr std::string_view kWallpaperDirectory = "assets/wallpapers";
// Shown when nothing was saved, or the saved image is gone.
inline constexpr std::string_view kDefaultWallpaper = "frieren.jpg";
// The choice that draws the gradient instead of an image. It has no image
// extension, so it can't clash with a file name.
inline constexpr std::string_view kGradientWallpaper = "gradient";

// Whether a file name ends in .jpg, .jpeg or .png, in any case.
[[nodiscard]] bool IsWallpaperFile(std::string_view file_name);

// The image file names in `directory`, sorted. Empty if it can't be read.
[[nodiscard]] std::vector<std::string> ListWallpapers(
    const std::filesystem::path& directory);

// "frieren.jpg" -> "frieren", "blue_waves.png" -> "blue waves": the caption
// under a thumbnail.
[[nodiscard]] std::string WallpaperLabel(std::string_view file_name);

// The wallpaper to show at launch: the saved choice if it is the gradient or
// still available, else the default if available, else the gradient.
[[nodiscard]] std::string ResolveWallpaper(
    const std::optional<std::string>& saved,
    std::span<const std::string> available);

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_WALLPAPER_CATALOG_H_
