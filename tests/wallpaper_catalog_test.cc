#include "data/wallpaper_catalog.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <ostream>  // IWYU pragma: keep (doctest prints std::string_view)
#include <string>
#include <vector>

#include <doctest/doctest.h>

namespace csopesy::data {
namespace {

// An empty folder in the system temp directory; each test removes it.
std::filesystem::path MakeTempFolder(const std::string& name) {
  const std::filesystem::path path =
      std::filesystem::temp_directory_path() / name;
  std::filesystem::remove_all(path);
  std::filesystem::create_directories(path);
  return path;
}

TEST_CASE("Only .jpg, .jpeg and .png files count as wallpapers") {
  CHECK(IsWallpaperFile("a.jpg"));
  CHECK(IsWallpaperFile("B.JPEG"));
  CHECK(IsWallpaperFile("c.Png"));
  CHECK_FALSE(IsWallpaperFile("notes.txt"));
  CHECK_FALSE(IsWallpaperFile(".png"));
  CHECK_FALSE(IsWallpaperFile("gradient"));
}

TEST_CASE("The wallpaper list keeps image files, sorted by name") {
  const std::filesystem::path folder =
      MakeTempFolder("csopesy_wallpaper_catalog_test");
  std::ofstream(folder / "zebra.png") << "x";
  std::ofstream(folder / "notes.txt") << "x";
  std::ofstream(folder / "apple.jpg") << "x";
  std::filesystem::create_directories(folder / "folder.jpg");
  const std::vector<std::string> names = ListWallpapers(folder);
  std::filesystem::remove_all(folder);
  REQUIRE(names.size() == 2);
  CHECK(names.front() == "apple.jpg");
  CHECK(names.back() == "zebra.png");
}

TEST_CASE("A missing wallpaper folder lists nothing") {
  CHECK(ListWallpapers("no/such/folder").empty());
}

TEST_CASE("Thumbnail captions drop the extension and underscores") {
  CHECK(WallpaperLabel("blue_waves.jpg") == "blue waves");
  CHECK(WallpaperLabel("frieren.jpg") == "frieren");
}

TEST_CASE("The saved wallpaper wins while it still exists") {
  const std::vector<std::string> available{"a.jpg", "frieren.jpg"};
  CHECK(ResolveWallpaper(std::string("a.jpg"), available) == "a.jpg");
  CHECK(ResolveWallpaper(std::string(kGradientWallpaper), available) ==
        kGradientWallpaper);
}

TEST_CASE("A missing saved wallpaper falls back to the default") {
  const std::vector<std::string> available{"a.jpg", "frieren.jpg"};
  CHECK(ResolveWallpaper(std::string("gone.png"), available) ==
        kDefaultWallpaper);
  CHECK(ResolveWallpaper(std::nullopt, available) == kDefaultWallpaper);
}

TEST_CASE("Without the default image the gradient is used") {
  const std::vector<std::string> available{"a.jpg"};
  CHECK(ResolveWallpaper(std::nullopt, available) == kGradientWallpaper);
}

}  // namespace
}  // namespace csopesy::data
