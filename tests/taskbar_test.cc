#include "shell/taskbar.h"

#include <cstddef>
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

void DrawFrame(const Taskbar& taskbar, WindowManager& manager) {
  HeadlessImGui::BeginFrame();
  manager.RenderAll(WorkAreaAboveTaskbar(MainViewportRect()));
  taskbar.Draw(manager);
  HeadlessImGui::EndFrame();
}

// Moves the mouse onto the app button at `index`, then presses and releases
// the left button, one input event per frame as ImGui expects.
void ClickAppButton(const Taskbar& taskbar, WindowManager& manager,
                    std::size_t index) {
  const ScreenRect button =
      TaskbarButtonRect(TaskbarRect(MainViewportRect()), index);
  const ImVec2 center = (button.min + button.max) * 0.5F;
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(center.x, center.y);
  DrawFrame(taskbar, manager);
  io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
  DrawFrame(taskbar, manager);
  io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
  DrawFrame(taskbar, manager);
}

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

TEST_CASE("App buttons sit in a row inside the taskbar") {
  const ScreenRect bar = TaskbarRect(kViewport);
  const ScreenRect first = TaskbarButtonRect(bar, 0);
  const ScreenRect second = TaskbarButtonRect(bar, 1);
  CHECK(first.min.x == bar.min.x + Theme::kTaskbarPadding);
  CHECK(first.min.y >= bar.min.y);
  CHECK(first.max.y <= bar.max.y);
  CHECK(first.max.x - first.min.x == Theme::kTaskbarButtonSize);
  CHECK(second.min.x - first.max.x == Theme::kTaskbarButtonSpacing);
  CHECK(second.min.y == first.min.y);
}

TEST_CASE("The taskbar is drawn in front of a focused app window") {
  const HeadlessImGui imgui;
  WindowManager manager;
  FakeAppWindow app;
  manager.Add(app);
  Taskbar taskbar;
  taskbar.Pin(app, TaskbarIcon::kFolder);
  manager.ToggleFromTaskbar(app);

  DrawFrame(taskbar, manager);

  const ImGuiContext& context = *ImGui::GetCurrentContext();
  REQUIRE_FALSE(context.Windows.empty());
  CHECK(std::string_view(context.Windows.back()->Name) == "##taskbar");
}

TEST_CASE("Indicators match which windows are open and which is active") {
  WindowManager manager;
  FakeAppWindow closed("Closed");
  FakeAppWindow behind("Behind");
  FakeAppWindow minimized("Minimized");
  FakeAppWindow front("Front");
  manager.Add(closed);
  manager.Add(behind);
  manager.Add(minimized);
  manager.Add(front);
  manager.ToggleFromTaskbar(behind);
  manager.ToggleFromTaskbar(minimized);
  minimized.Minimize();
  manager.ToggleFromTaskbar(front);

  CHECK(IndicatorFor(manager, closed) == IndicatorState::kHidden);
  CHECK(IndicatorFor(manager, behind) == IndicatorState::kRunning);
  CHECK(IndicatorFor(manager, minimized) == IndicatorState::kRunning);
  CHECK(IndicatorFor(manager, front) == IndicatorState::kActive);
}

TEST_CASE("Closing a window hides its indicator") {
  WindowManager manager;
  FakeAppWindow app;
  manager.Add(app);
  manager.ToggleFromTaskbar(app);
  CHECK(IndicatorFor(manager, app) == IndicatorState::kActive);

  app.Close();
  CHECK(IndicatorFor(manager, app) == IndicatorState::kHidden);
}

TEST_CASE("Clicking an app button opens, minimizes and restores its window") {
  const HeadlessImGui imgui;
  WindowManager manager;
  FakeAppWindow first("First");
  FakeAppWindow second("Second");
  manager.Add(first);
  manager.Add(second);
  Taskbar taskbar;
  taskbar.Pin(first, TaskbarIcon::kFolder);
  taskbar.Pin(second, TaskbarIcon::kTerminal);
  DrawFrame(taskbar, manager);

  ClickAppButton(taskbar, manager, 1);
  CHECK(second.is_open());
  CHECK(manager.IsActive(second));
  CHECK_FALSE(first.is_open());

  ClickAppButton(taskbar, manager, 1);
  CHECK(second.is_minimized());

  ClickAppButton(taskbar, manager, 1);
  CHECK_FALSE(second.is_minimized());
  CHECK(manager.IsActive(second));
}

}  // namespace
}  // namespace csopesy::shell
