#include "shell/window_manager.h"

#include "fake_app_window.h"
#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"

namespace csopesy::shell {
namespace {

using testing::FakeAppWindow;
using testing::HeadlessImGui;

constexpr apps::WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 660.0F),
};

void RenderOneFrame(WindowManager& manager) {
  HeadlessImGui::BeginFrame();
  manager.RenderAll(kArea);
  HeadlessImGui::EndFrame();
}

TEST_CASE("WindowManager taskbar click opens, minimizes and restores") {
  WindowManager manager;
  FakeAppWindow app;
  manager.Add(app);

  manager.ToggleFromTaskbar(app);
  CHECK(app.is_open());
  CHECK(manager.IsActive(app));

  manager.ToggleFromTaskbar(app);
  CHECK(app.is_minimized());
  CHECK_FALSE(manager.IsActive(app));

  manager.ToggleFromTaskbar(app);
  CHECK_FALSE(app.is_minimized());
  CHECK(manager.IsActive(app));
}

TEST_CASE("WindowManager taskbar click on a window behind brings it front") {
  WindowManager manager;
  FakeAppWindow first("First");
  FakeAppWindow second("Second");
  manager.Add(first);
  manager.Add(second);

  manager.ToggleFromTaskbar(first);
  manager.ToggleFromTaskbar(second);
  CHECK(manager.IsActive(second));

  manager.ToggleFromTaskbar(first);
  CHECK(manager.IsActive(first));
  CHECK_FALSE(first.is_minimized());
  CHECK_FALSE(manager.IsActive(second));
}

TEST_CASE("WindowManager running flags match open windows") {
  WindowManager manager;
  FakeAppWindow open_app("Open");
  FakeAppWindow minimized_app("Minimized");
  FakeAppWindow closed_app("Closed");
  manager.Add(open_app);
  manager.Add(minimized_app);
  manager.Add(closed_app);

  manager.ToggleFromTaskbar(open_app);
  manager.ToggleFromTaskbar(minimized_app);
  manager.ToggleFromTaskbar(minimized_app);

  CHECK(WindowManager::IsRunning(open_app));
  CHECK(WindowManager::IsRunning(minimized_app));
  CHECK_FALSE(WindowManager::IsRunning(closed_app));
}

TEST_CASE("WindowManager forgets the active window when it closes") {
  WindowManager manager;
  FakeAppWindow app;
  manager.Add(app);
  manager.ToggleFromTaskbar(app);
  app.Close();
  CHECK_FALSE(manager.IsActive(app));

  manager.ToggleFromTaskbar(app);
  CHECK(app.is_open());
  CHECK(manager.IsActive(app));
}

TEST_CASE("WindowManager staggers first-use positions") {
  WindowManager manager;
  FakeAppWindow first("First");
  FakeAppWindow second("Second");
  manager.Add(first);
  manager.Add(second);
  REQUIRE(manager.windows().size() == 2);
  CHECK(manager.windows().front() == &first);
  CHECK(manager.windows().back() == &second);
  CHECK(first.default_offset() != second.default_offset());
}

TEST_CASE("WindowManager renders open windows and follows ImGui focus") {
  const HeadlessImGui imgui;
  WindowManager manager;
  FakeAppWindow first("First");
  FakeAppWindow second("Second");
  manager.Add(first);
  manager.Add(second);

  manager.ToggleFromTaskbar(first);
  RenderOneFrame(manager);
  CHECK(first.draw_count() == 1);
  CHECK(second.draw_count() == 0);
  CHECK(manager.IsActive(first));

  manager.ToggleFromTaskbar(second);
  RenderOneFrame(manager);
  CHECK(second.draw_count() == 1);
  CHECK(manager.IsActive(second));
}

}  // namespace
}  // namespace csopesy::shell
