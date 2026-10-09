#include "core/theme.h"

#include <filesystem>
#include <string_view>

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

namespace csopesy::core {
namespace {

TEST_CASE("Theme loads a shell font and a separate boot font") {
  const testing::HeadlessImGui imgui;
  Theme theme;
  theme.Apply();

  REQUIRE(theme.shell_font() != nullptr);
  REQUIRE(theme.boot_font() != nullptr);
  CHECK(theme.shell_font() != theme.boot_font());
  CHECK(ImGui::GetIO().FontDefault == theme.shell_font());
}

TEST_CASE("Theme loads Roboto as the shell font when the file exists") {
  const testing::HeadlessImGui imgui;
  Theme theme;
  theme.Apply(std::filesystem::path(CSOPESY_SOURCE_DIR) /
              Theme::kShellFontPath);

  REQUIRE(theme.shell_font() != nullptr);
  CHECK(std::string_view(theme.shell_font()->GetDebugName())
            .starts_with("Roboto"));
}

TEST_CASE("Theme falls back to the built-in font when the file is missing") {
  const testing::HeadlessImGui imgui;
  Theme theme;
  theme.Apply("no/such/font.ttf");

  REQUIRE(theme.shell_font() != nullptr);
  CHECK(theme.shell_font() != theme.boot_font());
}

TEST_CASE("Theme makes window backgrounds opaque") {
  const testing::HeadlessImGui imgui;
  Theme theme;
  theme.Apply();

  CHECK(ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w == 1.0F);
}

TEST_CASE("Theme sets the shell font size") {
  const testing::HeadlessImGui imgui;
  Theme theme;
  theme.Apply();

  CHECK(ImGui::GetStyle().FontSizeBase == Theme::kShellFontSize);
}

}  // namespace
}  // namespace csopesy::core
