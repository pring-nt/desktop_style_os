#include "boot/splash_screen.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "imgui.h"

#include "core/state_machine.h"
#include "core/theme.h"
#include "shell/desktop.h"

namespace csopesy::boot {

namespace {

using core::Seconds;
using core::Theme;

constexpr int kMaxDots = 3;
constexpr float kHalf = 0.5F;
constexpr std::string_view kWidestLoadingText = "Loading...";
// Lines under the logo: blank, name, group credit, blank, loading.
constexpr std::size_t kTextLineCount = 5;

constexpr std::array<std::string_view, 5> kLogo{
    R"(  ____ ____   ___  ____  _____ ______   __)",
    R"( / ___/ ___| / _ \|  _ \| ____/ ___\ \ / /)",
    R"(| |   \___ \| | | | |_) |  _| \___ \\ V / )",
    R"(| |___ ___) | |_| |  __/| |___ ___) || |  )",
    R"( \____|____/ \___/|_|   |_____|____/ |_|  )",
};

struct TextLine {
  std::string text;
  ImVec4 color;
};

ImU32 WithOpacity(const ImVec4& color, float opacity) {
  return ImGui::GetColorU32(
      ImVec4(color.x, color.y, color.z, color.w * opacity));
}

}  // namespace

std::string LoadingText(Seconds elapsed) {
  const int steps = static_cast<int>(elapsed / kLoadingDotInterval);
  const int dots = 1 + (std::max(steps, 0) % kMaxDots);
  return "Loading" + std::string(static_cast<std::size_t>(dots), '.');
}

float SplashOpacity(Seconds elapsed) {
  return std::clamp(elapsed / kSplashFadeIn, 0.0F, 1.0F);
}

void SplashScreen::Draw(Seconds elapsed, ImFont* font) {
  const shell::ScreenRect viewport = shell::MainViewportRect();
  ImDrawList& draw_list = *ImGui::GetBackgroundDrawList();
  draw_list.AddRectFilled(viewport.min, viewport.max,
                          ImGui::GetColorU32(Theme::kBootBackgroundColor));

  std::vector<TextLine> lines;
  lines.reserve(kLogo.size() + kTextLineCount);
  const auto add = [&lines](std::string text, const ImVec4& color) {
    lines.push_back(TextLine{.text = std::move(text), .color = color});
  };
  for (const std::string_view row : kLogo) {
    add(std::string(row), Theme::kSplashLogoColor);
  }
  add("", Theme::kBootTextColor);
  add("CSOPESY Desktop OS Emulator v1.0", Theme::kBootTextColor);
  add("CSOPESY - Section S01 - Group 12", Theme::kSplashSubtitleColor);
  add("", Theme::kBootTextColor);
  // Padded to its longest form so the centered line does not shift as dots
  // appear.
  std::string loading = LoadingText(elapsed);
  loading.resize(kWidestLoadingText.size(), ' ');
  add(std::move(loading), Theme::kSplashLoadingColor);

  const float size = Theme::kBootFontSize * Theme::kBootTextScale;
  const float line_height = size * Theme::kBootLineSpacing;
  const float opacity = SplashOpacity(elapsed);
  const ImVec2 center = (viewport.min + viewport.max) * kHalf;
  float y = center.y - (line_height * static_cast<float>(lines.size()) * kHalf);
  for (const TextLine& line : lines) {
    const float width =
        font->CalcTextSizeA(size, viewport.max.x - viewport.min.x, 0.0F,
                            line.text.c_str())
            .x;
    draw_list.AddText(font, size, ImVec2(center.x - (width * kHalf), y),
                      WithOpacity(line.color, opacity), line.text.c_str());
    y += line_height;
  }
}

}  // namespace csopesy::boot
