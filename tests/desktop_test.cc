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
constexpr ImVec2 kWideImage{1920.0F, 1080.0F};
constexpr ImVec2 kSquareImage{1000.0F, 1000.0F};
constexpr ScreenRect kSquareViewport{
    .min = ImVec2(100.0F, 100.0F),
    .max = ImVec2(600.0F, 600.0F),
};
constexpr float kQuarter = 0.25F;
constexpr float kThreeQuarters = 0.75F;

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

TEST_CASE("DesktopLabelRect sits in the top-left corner") {
  const ScreenRect panel = DesktopLabelRect(kSquareViewport, kTextSize);
  CHECK(panel.min.x == kSquareViewport.min.x + Theme::kClockPanelMargin);
  CHECK(panel.min.y == kSquareViewport.min.y + Theme::kClockPanelMargin);
  CHECK(panel.max.x - panel.min.x ==
        kTextSize.x + kPanelPadding.x + kPanelPadding.x);
}

TEST_CASE("CoverUv shows the whole image when aspect ratios match") {
  const UvRect uv = CoverUv(kWideImage, kLargeViewport);
  CHECK(uv.min.x == doctest::Approx(0.0F));
  CHECK(uv.min.y == doctest::Approx(0.0F));
  CHECK(uv.max.x == doctest::Approx(1.0F));
  CHECK(uv.max.y == doctest::Approx(1.0F));
}

TEST_CASE("CoverUv crops the overflowing axis equally on both sides") {
  const UvRect wide_on_square = CoverUv(kWideImage, kSquareViewport);
  CHECK(wide_on_square.min.y == doctest::Approx(0.0F));
  CHECK(wide_on_square.max.y == doctest::Approx(1.0F));
  CHECK(wide_on_square.min.x == doctest::Approx(1.0F - wide_on_square.max.x));
  CHECK(wide_on_square.max.x - wide_on_square.min.x ==
        doctest::Approx(kWideImage.y / kWideImage.x));

  const ScreenRect wide_viewport{
      .min = ImVec2(0.0F, 0.0F),
      .max = ImVec2(kSquareImage.x, kSquareImage.y * 0.5F),
  };
  const UvRect square_on_wide = CoverUv(kSquareImage, wide_viewport);
  CHECK(square_on_wide.min.x == doctest::Approx(0.0F));
  CHECK(square_on_wide.max.x == doctest::Approx(1.0F));
  CHECK(square_on_wide.min.y == doctest::Approx(kQuarter));
  CHECK(square_on_wide.max.y == doctest::Approx(kThreeQuarters));
}

TEST_CASE("CoverUv falls back to the whole image for empty sizes") {
  const UvRect uv = CoverUv(ImVec2(0.0F, 0.0F), kSmallViewport);
  CHECK(uv.min.x == 0.0F);
  CHECK(uv.max.y == 1.0F);
}

TEST_CASE("A missing wallpaper file leaves the gradient fallback") {
  Desktop desktop;
  desktop.LoadWallpaper("no/such/wallpaper.jpg");
  CHECK_FALSE(desktop.has_wallpaper());
}

TEST_CASE("Desktop draws the wallpaper and clock on the background layer") {
  const testing::HeadlessImGui imgui;
  const auto fixed_time = std::chrono::system_clock::time_point{};
  const core::Clock clock([fixed_time] { return fixed_time; });
  const Desktop desktop;

  testing::HeadlessImGui::BeginFrame();
  desktop.Draw(clock);
  CHECK(ImGui::GetBackgroundDrawList()->VtxBuffer.Size > 0);
  testing::HeadlessImGui::EndFrame();
}

}  // namespace
}  // namespace csopesy::shell
