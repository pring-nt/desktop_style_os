#include "core/theme.h"

#include "imgui.h"

namespace csopesy::core {

namespace {

constexpr float kWindowRounding = 8.0F;
constexpr float kFrameRounding = 4.0F;
constexpr float kPopupRounding = 6.0F;
constexpr ImVec2 kWindowPadding{10.0F, 10.0F};
constexpr ImVec2 kFramePadding{8.0F, 4.0F};
constexpr ImVec2 kItemSpacing{8.0F, 6.0F};

}  // namespace

void Theme::Apply() {
  ImGuiIO& io = ImGui::GetIO();
  shell_font_ = io.Fonts->AddFontDefaultVector();
  boot_font_ = io.Fonts->AddFontDefaultBitmap();
  io.FontDefault = shell_font_;

  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::StyleColorsDark(&style);
  style.FontSizeBase = kShellFontSize;
  style.WindowRounding = kWindowRounding;
  style.ChildRounding = kFrameRounding;
  style.FrameRounding = kFrameRounding;
  style.GrabRounding = kFrameRounding;
  style.ScrollbarRounding = kFrameRounding;
  style.TabRounding = kFrameRounding;
  style.PopupRounding = kPopupRounding;
  style.WindowPadding = kWindowPadding;
  style.FramePadding = kFramePadding;
  style.ItemSpacing = kItemSpacing;
}

}  // namespace csopesy::core
