#include "shell/desktop.h"

#include <string>

#include "imgui.h"

#include "core/clock.h"
#include "core/theme.h"

namespace csopesy::shell {

using core::Theme;

ScreenRect ClockPanelRect(const ScreenRect& viewport, ImVec2 text_size) {
  const ImVec2 panel_size = text_size + (Theme::kClockPanelPadding * 2.0F);
  const ImVec2 max{viewport.max.x - Theme::kClockPanelMargin,
                   viewport.min.y + Theme::kClockPanelMargin + panel_size.y};
  return {.min = max - panel_size, .max = max};
}

void Desktop::Draw(const core::Clock& clock) {
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  const ScreenRect bounds{
      .min = viewport->Pos,
      .max = viewport->Pos + viewport->Size,
  };
  ImDrawList& draw_list = *ImGui::GetBackgroundDrawList();
  DrawWallpaper(draw_list, bounds);
  DrawClock(draw_list, bounds, clock);
}

void Desktop::DrawWallpaper(ImDrawList& draw_list, const ScreenRect& viewport) {
  const ImU32 top = ImGui::GetColorU32(Theme::kWallpaperTopColor);
  const ImU32 bottom = ImGui::GetColorU32(Theme::kWallpaperBottomColor);
  draw_list.AddRectFilledMultiColor(viewport.min, viewport.max, top, top,
                                    bottom, bottom);
}

void Desktop::DrawClock(ImDrawList& draw_list, const ScreenRect& viewport,
                        const core::Clock& clock) {
  const std::string text = clock.FormattedNow();
  const ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
  const ScreenRect panel = ClockPanelRect(viewport, text_size);
  draw_list.AddRectFilled(panel.min, panel.max,
                          ImGui::GetColorU32(Theme::kClockPanelColor),
                          Theme::kClockPanelRounding);
  draw_list.AddText(panel.min + Theme::kClockPanelPadding,
                    ImGui::GetColorU32(Theme::kClockTextColor), text.c_str());
}

}  // namespace csopesy::shell
