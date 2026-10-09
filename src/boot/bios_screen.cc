#include "boot/bios_screen.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"

#include "core/state_machine.h"
#include "core/theme.h"
#include "shell/desktop.h"

namespace csopesy::boot {

namespace {

using core::Seconds;
using core::Theme;

constexpr Seconds kCursorBlinkPeriod{0.5F};
// The cursor is shown for this share of each blink period.
constexpr float kCursorDutyCycle = 0.5F;
constexpr std::string_view kMemoryTestMarker = "@memory";

// "@memory" marks where the counting memory test line goes.
constexpr std::array<std::string_view, 17> kPostLines{
    "CSOPESY Modular BIOS v1.0, An Energy Star Ally",
    "Copyright (C) 2026, CSOPESY Systems, Inc.",
    "",
    "CSOPESY-ATX Mainboard, Release 10/09/2026",
    "",
    "Main Processor  : CSOPESY Virtual CPU 3.20GHz",
    "Processor Cores : 4 cores, 8 threads",
    kMemoryTestMarker,
    "",
    "Award Plug and Play BIOS Extension v1.0A",
    "Detecting IDE drives ...",
    "  Primary Master   : CSOPESY VDISK 512GB",
    "  Primary Slave    : None",
    "  Secondary Master : CSOPESY OPTICAL DRIVE",
    "  Secondary Slave  : None",
    "",
    "Press DEL to enter SETUP, any key to skip",
};

std::string MemoryTestLine(Seconds since_start) {
  const std::uint32_t counted = MemoryTestKb(since_start);
  const bool done = since_start >= kMemoryTestDuration;
  return std::format("Memory Test     : {}K{}", counted, done ? " OK" : "");
}

}  // namespace

std::uint32_t MemoryTestKb(Seconds elapsed) {
  const float progress = std::clamp(elapsed / kMemoryTestDuration, 0.0F, 1.0F);
  return static_cast<std::uint32_t>(
      std::floor(progress * static_cast<float>(kTotalMemoryKb)));
}

std::vector<std::string> VisibleBiosLines(Seconds elapsed) {
  std::vector<std::string> lines;
  Seconds appears_at{0.0F};
  for (const std::string_view line : kPostLines) {
    if (elapsed < appears_at) {
      break;
    }
    if (line == kMemoryTestMarker) {
      lines.push_back(MemoryTestLine(elapsed - appears_at));
      appears_at += kMemoryTestDuration;
    } else {
      lines.emplace_back(line);
    }
    appears_at += kBiosLineInterval;
  }
  return lines;
}

std::string FunFactFooter(std::string_view fact) {
  return std::format("Fun Fact: {}", fact);
}

bool BiosCursorVisible(Seconds elapsed) {
  return std::fmod(elapsed.count(), kCursorBlinkPeriod.count()) <
         kCursorBlinkPeriod.count() * kCursorDutyCycle;
}

void BiosScreen::Draw(Seconds elapsed, ImFont* font,
                      std::string_view fun_fact) {
  const shell::ScreenRect viewport = shell::MainViewportRect();
  ImDrawList& draw_list = *ImGui::GetBackgroundDrawList();
  draw_list.AddRectFilled(viewport.min, viewport.max,
                          ImGui::GetColorU32(Theme::kBootBackgroundColor));

  const float size = Theme::kBootFontSize * Theme::kBootTextScale;
  const float line_height = size * Theme::kBootLineSpacing;
  const ImU32 color = ImGui::GetColorU32(Theme::kBootTextColor);
  std::vector<std::string> lines = VisibleBiosLines(elapsed);
  if (!lines.empty() && BiosCursorVisible(elapsed)) {
    lines.back() += '_';
  }
  ImVec2 position = viewport.min + Theme::kBootMargin;
  for (const std::string& line : lines) {
    draw_list.AddText(font, size, position, color, line.c_str());
    position.y += line_height;
  }

  // Wrapped to the window width and anchored to the bottom margin.
  const std::string footer = FunFactFooter(fun_fact);
  const float wrap_width =
      viewport.max.x - viewport.min.x - (Theme::kBootMargin.x * 2.0F);
  const float footer_height =
      font->CalcTextSizeA(size, FLT_MAX, wrap_width, footer.c_str()).y;
  const ImVec2 footer_position{
      viewport.min.x + Theme::kBootMargin.x,
      viewport.max.y - Theme::kBootMargin.y - footer_height};
  draw_list.AddText(font, size, footer_position,
                    ImGui::GetColorU32(Theme::kBiosFunFactColor),
                    footer.c_str(), nullptr, wrap_width);
}

}  // namespace csopesy::boot
