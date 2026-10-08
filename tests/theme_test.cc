#include "core/theme.h"

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

TEST_CASE("Theme sets the shell font size") {
  const testing::HeadlessImGui imgui;
  Theme theme;
  theme.Apply();

  CHECK(ImGui::GetStyle().FontSizeBase == Theme::kShellFontSize);
}

}  // namespace
}  // namespace csopesy::core
