#include "core/theme.h"

#include "imgui.h"
#include <doctest/doctest.h>

namespace csopesy::core {
namespace {

// Theme only touches the font atlas and style, so a bare ImGui context is
// enough; no window or GPU is needed.
class ImGuiContextScope {
 public:
  ImGuiContextScope() { ImGui::CreateContext(); }
  ~ImGuiContextScope() { ImGui::DestroyContext(); }
  ImGuiContextScope(const ImGuiContextScope&) = delete;
  ImGuiContextScope& operator=(const ImGuiContextScope&) = delete;
  ImGuiContextScope(ImGuiContextScope&&) = delete;
  ImGuiContextScope& operator=(ImGuiContextScope&&) = delete;
};

TEST_CASE("Theme loads a shell font and a separate boot font") {
  const ImGuiContextScope context;
  Theme theme;
  theme.Apply();

  REQUIRE(theme.shell_font() != nullptr);
  REQUIRE(theme.boot_font() != nullptr);
  CHECK(theme.shell_font() != theme.boot_font());
  CHECK(ImGui::GetIO().FontDefault == theme.shell_font());
}

TEST_CASE("Theme sets the shell font size") {
  const ImGuiContextScope context;
  Theme theme;
  theme.Apply();

  CHECK(ImGui::GetStyle().FontSizeBase == Theme::kShellFontSize);
}

}  // namespace
}  // namespace csopesy::core
