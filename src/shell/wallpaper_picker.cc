#include "shell/wallpaper_picker.h"

#include <algorithm>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "imgui.h"

#include "core/texture.h"
#include "core/theme.h"
#include "data/wallpaper_catalog.h"
#include "shell/desktop.h"

namespace csopesy::shell {

namespace {

using core::Theme;

constexpr const char* kHint =
    "Pick a wallpaper. Add .jpg or .png files to assets/wallpapers to see "
    "them here.";

constexpr float kHalf = 0.5F;

// Puts the next tile beside the previous one if it fits before `right_edge`,
// else starts a new row.
void FlowNextTile(float right_edge) {
  const float next_right = ImGui::GetItemRectMax().x +
                           ImGui::GetStyle().ItemSpacing.x +
                           Theme::kWallpaperThumbSize.x;
  if (next_right <= right_edge) {
    ImGui::SameLine();
  }
}

void DrawGradientPreview(ImDrawList& draw_list, ImVec2 min, ImVec2 max) {
  const ImU32 top = ImGui::GetColorU32(Theme::kWallpaperTopColor);
  const ImU32 bottom = ImGui::GetColorU32(Theme::kWallpaperBottomColor);
  draw_list.AddRectFilledMultiColor(min, max, top, top, bottom, bottom);
}

}  // namespace

void WallpaperPicker::Rescan() {
  thumbnails_.clear();
  for (std::string& name : data::ListWallpapers(directory_)) {
    thumbnails_.push_back(Thumbnail{.name = std::move(name), .texture = {}});
  }
}

void WallpaperPicker::LoadThumbnails() {
  for (Thumbnail& thumbnail : thumbnails_) {
    thumbnail.texture =
        core::Texture::LoadFromFile(directory_ / thumbnail.name);
  }
}

void WallpaperPicker::ReleaseThumbnails() {
  for (Thumbnail& thumbnail : thumbnails_) {
    thumbnail.texture.reset();
  }
}

std::vector<std::string> WallpaperPicker::wallpapers() const {
  std::vector<std::string> names;
  names.reserve(thumbnails_.size());
  for (const Thumbnail& thumbnail : thumbnails_) {
    names.push_back(thumbnail.name);
  }
  return names;
}

std::optional<std::string> WallpaperPicker::TakeSelection() {
  return std::exchange(selection_, std::nullopt);
}

void WallpaperPicker::Draw() {
  ImGui::TextWrapped("%s", kHint);
  ImGui::Spacing();
  const float right_edge =
      ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x;
  const std::string gradient(data::kGradientWallpaper);
  if (DrawTile(gradient, nullptr)) {
    selection_ = gradient;
    current_ = gradient;
  }
  for (const Thumbnail& thumbnail : thumbnails_) {
    FlowNextTile(right_edge);
    const core::Texture* texture =
        thumbnail.texture ? &*thumbnail.texture : nullptr;
    if (DrawTile(thumbnail.name, texture)) {
      selection_ = thumbnail.name;
      current_ = thumbnail.name;
    }
  }
}

bool WallpaperPicker::DrawTile(const std::string& name,
                               const core::Texture* texture) const {
  const ImVec2 thumb = Theme::kWallpaperThumbSize;
  const float caption_height = ImGui::GetTextLineHeightWithSpacing();
  ImGui::PushID(name.c_str());
  const ImVec2 min = ImGui::GetCursorScreenPos();
  const bool clicked = ImGui::InvisibleButton(
      "##tile", ImVec2(thumb.x, thumb.y + caption_height));
  const bool hovered = ImGui::IsItemHovered();
  ImGui::PopID();

  ImDrawList& draw_list = *ImGui::GetWindowDrawList();
  const ImVec2 max = min + thumb;
  if (texture != nullptr) {
    const UvRect uv = CoverUv(texture->size(), {.min = min, .max = max});
    draw_list.AddImageRounded(texture->id(), min, max, uv.min, uv.max,
                              IM_COL32_WHITE, Theme::kWallpaperThumbRounding);
  } else if (name == data::kGradientWallpaper) {
    DrawGradientPreview(draw_list, min, max);
  } else {
    draw_list.AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_FrameBg),
                            Theme::kWallpaperThumbRounding);
  }
  if (name == current_) {
    draw_list.AddRect(
        min, max, ImGui::GetColorU32(Theme::kIndicatorActiveColor),
        Theme::kWallpaperThumbRounding, Theme::kWallpaperSelectedThickness);
  } else if (hovered) {
    draw_list.AddRect(min, max, ImGui::GetColorU32(ImGuiCol_ButtonHovered),
                      Theme::kWallpaperThumbRounding);
  }

  const std::string label = name == data::kGradientWallpaper
                                ? std::string("Gradient")
                                : data::WallpaperLabel(name);
  const float label_width = ImGui::CalcTextSize(label.c_str()).x;
  const ImVec2 label_pos{
      min.x + std::max(0.0F, (thumb.x - label_width) * kHalf),
      max.y + ImGui::GetStyle().ItemSpacing.y};
  draw_list.PushClipRect(ImVec2(min.x, max.y),
                         ImVec2(max.x, max.y + caption_height), true);
  draw_list.AddText(label_pos, ImGui::GetColorU32(ImGuiCol_Text),
                    label.c_str());
  draw_list.PopClipRect();
  return clicked;
}

}  // namespace csopesy::shell
