#include "shell/wallpaper_picker.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"

namespace csopesy::shell {
namespace {

using testing::HeadlessImGui;

constexpr int kFrames = 3;
constexpr apps::WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 664.0F),
};

TEST_CASE("The wallpaper picker lists the folder and draws without textures") {
  const std::filesystem::path folder =
      std::filesystem::temp_directory_path() / "csopesy_wallpaper_picker_test";
  std::filesystem::remove_all(folder);
  std::filesystem::create_directories(folder);
  std::ofstream(folder / "b.png") << "x";
  std::ofstream(folder / "a.jpg") << "x";

  const HeadlessImGui imgui;
  WallpaperPicker picker(folder);
  picker.Rescan();
  picker.set_current("a.jpg");
  picker.Open();
  for (int frame = 0; frame < kFrames; ++frame) {
    HeadlessImGui::BeginFrame();
    picker.Render(kArea);
    HeadlessImGui::EndFrame();
  }
  const std::vector<std::string> expected{"a.jpg", "b.png"};
  CHECK(picker.wallpapers() == expected);
  CHECK_FALSE(picker.TakeSelection().has_value());
  CHECK(picker.current() == "a.jpg");
  std::filesystem::remove_all(folder);
}

}  // namespace
}  // namespace csopesy::shell
