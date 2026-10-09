#include "shell/desktop.h"

#include <algorithm>
#include <filesystem>
#include <string>

#include "imgui.h"

#include "core/clock.h"
#include "core/texture.h"
#include "core/theme.h"

namespace csopesy::shell {

using core::Theme;

namespace {

constexpr float kHalf = 0.5F;

}  // namespace

ScreenRect MainViewportRect() {
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  return {
      .min = viewport->Pos,
      .max = viewport->Pos + viewport->Size,
  };
}

ScreenRect ClockPanelRect(const ScreenRect& viewport, ImVec2 text_size) {
  const ImVec2 panel_size = text_size + (Theme::kClockPanelPadding * 2.0F);
  const ImVec2 max{viewport.max.x - Theme::kClockPanelMargin,
                   viewport.min.y + Theme::kClockPanelMargin + panel_size.y};
  return {.min = max - panel_size, .max = max};
}

ScreenRect DesktopLabelRect(const ScreenRect& viewport, ImVec2 content_size) {
  const ImVec2 panel_size = content_size + (Theme::kClockPanelPadding * 2.0F);
  const ImVec2 min =
      viewport.min + ImVec2(Theme::kClockPanelMargin, Theme::kClockPanelMargin);
  return {.min = min, .max = min + panel_size};
}

UvRect CoverUv(ImVec2 image_size, const ScreenRect& viewport) {
  constexpr UvRect kFullImage{
      .min = ImVec2(0.0F, 0.0F),
      .max = ImVec2(1.0F, 1.0F),
  };
  const ImVec2 viewport_size = viewport.max - viewport.min;
  if (image_size.x <= 0.0F || image_size.y <= 0.0F || viewport_size.x <= 0.0F ||
      viewport_size.y <= 0.0F) {
    return kFullImage;
  }
  const float scale =
      std::max(viewport_size.x / image_size.x, viewport_size.y / image_size.y);
  const ImVec2 visible{viewport_size.x / (image_size.x * scale),
                       viewport_size.y / (image_size.y * scale)};
  const ImVec2 margin = (kFullImage.max - visible) * 0.5F;
  return {.min = margin, .max = kFullImage.max - margin};
}

void Desktop::LoadWallpaper(const std::filesystem::path& path) {
  wallpaper_ = core::Texture::LoadFromFile(path);
}

void Desktop::ReleaseWallpaper() { wallpaper_.reset(); }

void Desktop::Draw(const core::Clock& clock) const {
  const ScreenRect bounds = MainViewportRect();
  ImDrawList& draw_list = *ImGui::GetBackgroundDrawList();
  DrawWallpaper(draw_list, bounds);
  DrawLabel(draw_list, bounds);
  DrawClock(draw_list, bounds, clock);
}

void Desktop::DrawWallpaper(ImDrawList& draw_list,
                            const ScreenRect& viewport) const {
  if (!wallpaper_) {
    DrawGradient(draw_list, viewport);
    return;
  }
  const UvRect uv = CoverUv(wallpaper_->size(), viewport);
  draw_list.AddImage(wallpaper_->id(), viewport.min, viewport.max, uv.min,
                     uv.max);
}

void Desktop::DrawGradient(ImDrawList& draw_list, const ScreenRect& viewport) {
  const ImU32 top = ImGui::GetColorU32(Theme::kWallpaperTopColor);
  const ImU32 bottom = ImGui::GetColorU32(Theme::kWallpaperBottomColor);
  draw_list.AddRectFilledMultiColor(viewport.min, viewport.max, top, top,
                                    bottom, bottom);
}

void Desktop::DrawLabel(ImDrawList& draw_list, const ScreenRect& viewport) {
  const std::string text(kDesktopLabel);
  const ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
  // A green "online" dot, one line high, sits before the text.
  const float dot_space = text_size.y + Theme::kDesktopLabelDotGap;
  const ScreenRect panel =
      DesktopLabelRect(viewport, ImVec2(dot_space + text_size.x, text_size.y));
  draw_list.AddRectFilled(panel.min, panel.max,
                          ImGui::GetColorU32(Theme::kClockPanelColor),
                          Theme::kClockPanelRounding);
  const ImVec2 content = panel.min + Theme::kClockPanelPadding;
  const float radius = text_size.y * Theme::kDesktopLabelDotScale;
  draw_list.AddCircleFilled(
      content + (ImVec2(text_size.y, text_size.y) * kHalf), radius,
      ImGui::GetColorU32(Theme::kDesktopLabelDotColor));
  draw_list.AddText(content + ImVec2(dot_space, 0.0F),
                    ImGui::GetColorU32(Theme::kClockTextColor), text.c_str());
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
