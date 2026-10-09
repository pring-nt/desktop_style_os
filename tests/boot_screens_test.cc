#include <algorithm>
#include <cstddef>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "boot/bios_screen.h"
#include "boot/splash_screen.h"
#include "core/state_machine.h"
#include "core/theme.h"

namespace csopesy::boot {
namespace {

using core::Seconds;
using testing::HeadlessImGui;

constexpr Seconds kStart{0.0F};
constexpr Seconds kLongAfter{60.0F};
constexpr Seconds kHalfTest = kMemoryTestDuration * 0.5F;
constexpr std::size_t kAllPostLines = 17;

TEST_CASE("The memory test counts up and stops at the total") {
  CHECK(MemoryTestKb(kStart) == 0);
  CHECK(MemoryTestKb(kHalfTest) == kTotalMemoryKb / 2);
  CHECK(MemoryTestKb(kMemoryTestDuration) == kTotalMemoryKb);
  CHECK(MemoryTestKb(kLongAfter) == kTotalMemoryKb);
}

TEST_CASE("BIOS lines appear one at a time") {
  CHECK(VisibleBiosLines(kStart).size() == 1);
  CHECK(VisibleBiosLines(kBiosLineInterval).size() == 2);
  CHECK(VisibleBiosLines(kStart).front() ==
        "CSOPESY Modular BIOS v1.0, An Energy Star Ally");
}

TEST_CASE("Every BIOS line is shown once the POST finishes") {
  const std::vector<std::string> lines = VisibleBiosLines(kLongAfter);
  REQUIRE(lines.size() == kAllPostLines);
  CHECK(lines.back() == "Press DEL to enter SETUP, any key to skip");
}

TEST_CASE("The memory line ends with OK only after the count finishes") {
  const std::vector<std::string> done = VisibleBiosLines(kLongAfter);
  CHECK(std::ranges::find(done, "Memory Test     : 64000K OK") != done.end());
}

TEST_CASE("The whole POST fits in the default BIOS duration") {
  const std::vector<std::string> lines =
      VisibleBiosLines(core::kDefaultBiosDuration);
  CHECK(lines.size() == kAllPostLines);
}

TEST_CASE("The loading text cycles through one to three dots") {
  CHECK(LoadingText(kStart) == "Loading.");
  CHECK(LoadingText(kLoadingDotInterval) == "Loading..");
  CHECK(LoadingText(kLoadingDotInterval + kLoadingDotInterval) == "Loading...");
  CHECK(LoadingText(kLoadingDotInterval + kLoadingDotInterval +
                    kLoadingDotInterval) == "Loading.");
}

TEST_CASE("The splash fades in") {
  CHECK(SplashOpacity(kStart) == 0.0F);
  CHECK(SplashOpacity(kSplashFadeIn) == 1.0F);
  CHECK(SplashOpacity(kLongAfter) == 1.0F);
}

TEST_CASE("Both boot screens draw on the background layer") {
  const HeadlessImGui imgui;
  core::Theme theme;
  theme.Apply();
  HeadlessImGui::BeginFrame();
  BiosScreen::Draw(kLongAfter, theme.boot_font());
  SplashScreen::Draw(kLongAfter, theme.boot_font());
  CHECK(ImGui::GetBackgroundDrawList()->VtxBuffer.Size > 0);
  HeadlessImGui::EndFrame();
}

}  // namespace
}  // namespace csopesy::boot
