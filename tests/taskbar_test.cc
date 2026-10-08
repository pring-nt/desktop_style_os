#include "shell/taskbar.h"

#include <string_view>

#include "fake_app_window.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"
#include "core/theme.h"
#include "shell/desktop.h"
#include "shell/window_manager.h"

namespace csopesy::shell {
namespace {

using core::Theme;
using testing::FakeAppWindow;
using testing::HeadlessImGui;

constexpr ScreenRect kViewport{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 720.0F),
};
constexpr ScreenRect kResizedViewport{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(900.0F, 560.0F),
};

TEST_CASE("TaskbarRect spans the full width at the bottom") {
  const ScreenRect bar = TaskbarRect(kViewport);
  CHECK(bar.min.x == kViewport.min.x);
  CHECK(bar.max.x == kViewport.max.x);
  CHECK(bar.max.y == kViewport.max.y);
  CHECK(bar.max.y - bar.min.y == Theme::kTaskbarHeight);
}

TEST_CASE("TaskbarRect stays at the bottom after a resize") {
  const ScreenRect bar = TaskbarRect(kResizedViewport);
  CHECK(bar.max.x == kResizedViewport.max.x);
  CHECK(bar.max.y == kResizedViewport.max.y);
  CHECK(bar.max.y - bar.min.y == Theme::kTaskbarHeight);
}

TEST_CASE("The work area ends where the taskbar begins") {
  const apps::WorkArea area = WorkAreaAboveTaskbar(kViewport);
  CHECK(area.min.x == kViewport.min.x);
  CHECK(area.min.y == kViewport.min.y);
  CHECK(area.max.x == kViewport.max.x);
  CHECK(area.max.y == TaskbarRect(kViewport).min.y);
}

TEST_CASE("The taskbar is drawn in front of a focused app window") {
  const HeadlessImGui imgui;
  WindowManager manager;
  FakeAppWindow app;
  manager.Add(app);
  manager.ToggleFromTaskbar(app);

  HeadlessImGui::BeginFrame();
  manager.RenderAll(WorkAreaAboveTaskbar(MainViewportRect()));
  Taskbar::Draw();
  HeadlessImGui::EndFrame();

  const ImGuiContext& context = *ImGui::GetCurrentContext();
  REQUIRE_FALSE(context.Windows.empty());
  CHECK(std::string_view(context.Windows.back()->Name) == "##taskbar");
}

}  // namespace
}  // namespace csopesy::shell
