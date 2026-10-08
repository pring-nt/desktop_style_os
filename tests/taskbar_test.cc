#include "shell/taskbar.h"

#include <cstddef>
#include <functional>
#include <string_view>

#include "fake_app_window.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"
#include "core/state_machine.h"
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

void DrawFrame(const Taskbar& taskbar, WindowManager& manager,
               core::StateMachine& machine) {
  HeadlessImGui::BeginFrame();
  manager.RenderAll(WorkAreaAboveTaskbar(MainViewportRect()));
  taskbar.Draw(manager, machine);
  HeadlessImGui::EndFrame();
}

constexpr float kHalf = 0.5F;

ImVec2 Center(const ScreenRect& rect) { return (rect.min + rect.max) * kHalf; }

ImVec2 AppButtonCenter(std::size_t index) {
  return Center(TaskbarButtonRect(TaskbarRect(MainViewportRect()), index));
}

ImVec2 PowerButtonCenter() {
  return Center(
      TrayButtonRect(TaskbarRect(MainViewportRect()), TrayButton::kPower));
}

// Moves the mouse to `point`, then presses and releases the left button, one
// input event per frame as ImGui expects.
void ClickAt(ImVec2 point, const std::function<void()>& draw_frame) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddMousePosEvent(point.x, point.y);
  draw_frame();
  io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
  draw_frame();
  io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
  draw_frame();
}

void PressKey(ImGuiKey key, const std::function<void()>& draw_frame) {
  ImGuiIO& io = ImGui::GetIO();
  io.AddKeyEvent(key, true);
  draw_frame();
  io.AddKeyEvent(key, false);
  draw_frame();
}

core::StateMachine MakeMachineOnDesktop() {
  core::StateMachine machine;
  machine.SkipBios();
  machine.Update(core::kDefaultSplashDuration);
  return machine;
}

[[nodiscard]] bool IsAnyPopupOpen() {
  return ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId);
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
  core::StateMachine machine = MakeMachineOnDesktop();
  manager.ToggleFromTaskbar(app);

  DrawFrame(taskbar, manager, machine);

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

TEST_CASE("Clicking an app button opens only its own window") {
  const HeadlessImGui imgui;
  WindowManager manager;
  FakeAppWindow first("First");
  FakeAppWindow second("Second");
  manager.Add(first);
  manager.Add(second);
  Taskbar taskbar;
  taskbar.Pin(first, TaskbarIcon::kFolder);
  taskbar.Pin(second, TaskbarIcon::kTerminal);
  core::StateMachine machine = MakeMachineOnDesktop();
  const auto draw = [&] { DrawFrame(taskbar, manager, machine); };
  draw();

  ClickAt(AppButtonCenter(1), draw);
  CHECK(second.is_open());
  CHECK(manager.IsActive(second));
  CHECK_FALSE(first.is_open());
}

TEST_CASE("Clicking the active app's button minimizes, then restores it") {
  const HeadlessImGui imgui;
  WindowManager manager;
  FakeAppWindow app;
  manager.Add(app);
  Taskbar taskbar;
  taskbar.Pin(app, TaskbarIcon::kFolder);
  core::StateMachine machine = MakeMachineOnDesktop();
  const auto draw = [&] { DrawFrame(taskbar, manager, machine); };
  draw();
  ClickAt(AppButtonCenter(0), draw);

  ClickAt(AppButtonCenter(0), draw);
  CHECK(app.is_minimized());

  ClickAt(AppButtonCenter(0), draw);
  CHECK_FALSE(app.is_minimized());
  CHECK(manager.IsActive(app));
}

TEST_CASE("Tray buttons are right-aligned with PWR last") {
  const ScreenRect bar = TaskbarRect(kViewport);
  const ScreenRect volume = TrayButtonRect(bar, TrayButton::kVolume);
  const ScreenRect network = TrayButtonRect(bar, TrayButton::kNetwork);
  const ScreenRect power = TrayButtonRect(bar, TrayButton::kPower);
  CHECK(power.max.x == bar.max.x - Theme::kTaskbarPadding);
  CHECK(network.max.x + Theme::kTrayButtonSpacing == power.min.x);
  CHECK(volume.max.x + Theme::kTrayButtonSpacing == network.min.x);
  CHECK(power.min.y >= bar.min.y);
  CHECK(power.max.y <= bar.max.y);
}

TEST_CASE("PWR opens the shutdown dialog in front of the taskbar") {
  const HeadlessImGui imgui;
  WindowManager manager;
  const Taskbar taskbar;
  core::StateMachine machine = MakeMachineOnDesktop();
  const auto draw = [&] { DrawFrame(taskbar, manager, machine); };
  draw();

  ClickAt(PowerButtonCenter(), draw);
  draw();
  CHECK(machine.is_shutdown_pending());
  CHECK(IsAnyPopupOpen());
  const std::string_view front_window =
      ImGui::GetCurrentContext()->Windows.back()->Name;
  CHECK(front_window.starts_with("Shut Down"));
}

TEST_CASE("Escape in the shutdown dialog cancels back to the desktop") {
  const HeadlessImGui imgui;
  WindowManager manager;
  const Taskbar taskbar;
  core::StateMachine machine = MakeMachineOnDesktop();
  const auto draw = [&] { DrawFrame(taskbar, manager, machine); };
  draw();
  ClickAt(PowerButtonCenter(), draw);

  PressKey(ImGuiKey_Escape, draw);
  CHECK_FALSE(machine.is_shutdown_pending());
  CHECK(machine.state() == core::AppState::kDesktop);
  CHECK_FALSE(IsAnyPopupOpen());
}

}  // namespace
}  // namespace csopesy::shell
