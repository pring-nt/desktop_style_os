#include "apps/app_window.h"

#include "fake_app_window.h"
#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

namespace csopesy::apps {
namespace {

// A 1280 x 720 viewport with a 60 px taskbar along the bottom.
constexpr WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 660.0F),
};
using testing::FakeAppWindow;

constexpr ImVec2 kWindowSize = FakeAppWindow::kDefaultSize;
constexpr float kTitleBarHeight = 20.0F;

constexpr ImVec2 kInside{100.0F, 100.0F};
constexpr ImVec2 kBelowTaskbarTop{100.0F, 700.0F};
constexpr ImVec2 kAboveTopEdge{100.0F, -50.0F};
constexpr ImVec2 kFarLeft{-1000.0F, 100.0F};
constexpr ImVec2 kFarRight{5000.0F, 100.0F};

ImVec2 Clamp(ImVec2 position) {
  return ClampToWorkArea(position, kWindowSize, kArea, kTitleBarHeight);
}

void RenderOneFrame(AppWindow& app) {
  testing::HeadlessImGui::BeginFrame();
  app.Render(kArea);
  testing::HeadlessImGui::EndFrame();
}

TEST_CASE("ClampToWorkArea leaves a window inside the area alone") {
  CHECK(Clamp(kInside) == kInside);
}

TEST_CASE("ClampToWorkArea keeps the title bar above the taskbar") {
  const ImVec2 clamped = Clamp(kBelowTaskbarTop);
  CHECK(clamped.y == kArea.max.y - kTitleBarHeight);
  CHECK(clamped.x == kBelowTaskbarTop.x);
}

TEST_CASE("ClampToWorkArea keeps the title bar below the top edge") {
  CHECK(Clamp(kAboveTopEdge).y == kArea.min.y);
}

TEST_CASE("ClampToWorkArea keeps part of the window visible sideways") {
  CHECK(Clamp(kFarLeft).x == -kWindowSize.x + kMinVisibleWidth);
  CHECK(Clamp(kFarRight).x == kArea.max.x - kMinVisibleWidth);
}

TEST_CASE("AppWindow starts closed") {
  const FakeAppWindow app;
  CHECK(app.title() == "Fake");
  CHECK_FALSE(app.is_open());
  CHECK_FALSE(app.is_minimized());
}

TEST_CASE("AppWindow open, minimize, restore and close") {
  FakeAppWindow app;
  app.Open();
  CHECK(app.is_open());
  app.Minimize();
  CHECK(app.is_open());
  CHECK(app.is_minimized());
  app.Open();
  CHECK_FALSE(app.is_minimized());
  app.Close();
  CHECK_FALSE(app.is_open());
  CHECK_FALSE(app.is_minimized());
}

TEST_CASE("AppWindow draws its contents and takes focus when opened") {
  const testing::HeadlessImGui imgui;
  FakeAppWindow app;
  app.Open();
  RenderOneFrame(app);
  CHECK(app.draw_count() == 1);
  CHECK(app.is_focused());
}

TEST_CASE("AppWindow skips drawing while closed or minimized") {
  const testing::HeadlessImGui imgui;
  FakeAppWindow app;
  RenderOneFrame(app);
  app.Open();
  app.Minimize();
  RenderOneFrame(app);
  CHECK(app.draw_count() == 0);
  CHECK_FALSE(app.is_focused());
}

}  // namespace
}  // namespace csopesy::apps
