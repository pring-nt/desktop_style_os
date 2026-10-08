#include "shell/desktop.h"

#include <chrono>

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "core/clock.h"
#include "core/theme.h"

namespace csopesy::shell {
namespace {

using core::Theme;

constexpr ScreenRect kSmallViewport{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(800.0F, 600.0F),
};
constexpr ScreenRect kLargeViewport{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1920.0F, 1080.0F),
};
constexpr ImVec2 kPanelPadding = Theme::kClockPanelPadding;
constexpr ImVec2 kTextSize{200.0F, 16.0F};

TEST_CASE("ClockPanelRect sits in the top-right corner") {
  const ScreenRect panel = ClockPanelRect(kSmallViewport, kTextSize);
  CHECK(panel.max.x == kSmallViewport.max.x - Theme::kClockPanelMargin);
  CHECK(panel.min.y == kSmallViewport.min.y + Theme::kClockPanelMargin);
  CHECK(panel.max.x - panel.min.x ==
        kTextSize.x + kPanelPadding.x + kPanelPadding.x);
  CHECK(panel.max.y - panel.min.y ==
        kTextSize.y + kPanelPadding.y + kPanelPadding.y);
}

TEST_CASE("ClockPanelRect follows the corner when the window resizes") {
  const ScreenRect small = ClockPanelRect(kSmallViewport, kTextSize);
  const ScreenRect large = ClockPanelRect(kLargeViewport, kTextSize);
  CHECK(large.max.x - small.max.x ==
        kLargeViewport.max.x - kSmallViewport.max.x);
  CHECK(large.min.y == small.min.y);
}

TEST_CASE("Desktop draws the wallpaper and clock on the background layer") {
  const testing::HeadlessImGui imgui;
  const auto fixed_time = std::chrono::system_clock::time_point{};
  const core::Clock clock([fixed_time] { return fixed_time; });

  testing::HeadlessImGui::BeginFrame();
  Desktop::Draw(clock);
  CHECK(ImGui::GetBackgroundDrawList()->VtxBuffer.Size > 0);
  testing::HeadlessImGui::EndFrame();
}

}  // namespace
}  // namespace csopesy::shell
