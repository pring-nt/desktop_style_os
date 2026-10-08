#include "apps/app_window.h"

#include <algorithm>
#include <string>
#include <utility>

#include "imgui.h"

namespace csopesy::apps {

namespace {

constexpr ImGuiWindowFlags kAppWindowFlags = ImGuiWindowFlags_NoCollapse;
constexpr ImVec2 kMinWindowSize{240.0F, 160.0F};
constexpr float kMinimizeGlyphThickness = 1.5F;

}  // namespace

ImVec2 ClampToWorkArea(ImVec2 position, ImVec2 size, const WorkArea& area,
                       float title_bar_height) {
  const float min_x = area.min.x - size.x + kMinVisibleWidth;
  const float max_x = std::max(min_x, area.max.x - kMinVisibleWidth);
  const float min_y = area.min.y;
  const float max_y = std::max(min_y, area.max.y - title_bar_height);
  return {std::clamp(position.x, min_x, max_x),
          std::clamp(position.y, min_y, max_y)};
}

AppWindow::AppWindow(std::string title, ImVec2 default_size)
    : title_(std::move(title)), default_size_(default_size) {}

void AppWindow::Render(const WorkArea& area) {
  if (!is_open_ || is_minimized_) {
    is_focused_ = false;
    return;
  }

  ImGui::SetNextWindowPos(area.min + default_offset_, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(default_size_, ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints(kMinWindowSize, area.max - area.min);
  if (focus_requested_) {
    ImGui::SetNextWindowFocus();
    focus_requested_ = false;
  }

  bool keep_open = true;
  if (ImGui::Begin(title_.c_str(), &keep_open, kAppWindowFlags)) {
    const ImVec2 position = ImGui::GetWindowPos();
    const ImVec2 clamped = ClampToWorkArea(position, ImGui::GetWindowSize(),
                                           area, ImGui::GetFrameHeight());
    if (clamped != position) {
      ImGui::SetWindowPos(clamped);
    }
    Draw();
    // Drawn after the contents: it moves the cursor into the title bar, and
    // submitting the button last leaves the content layout untouched.
    if (DrawMinimizeButton()) {
      Minimize();
    }
  }
  is_focused_ = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
  ImGui::End();

  if (!keep_open) {
    Close();
  }
}

void AppWindow::Open() {
  is_open_ = true;
  is_minimized_ = false;
  focus_requested_ = true;
}

void AppWindow::Minimize() {
  is_minimized_ = true;
  is_focused_ = false;
}

void AppWindow::Close() {
  is_open_ = false;
  is_minimized_ = false;
  is_focused_ = false;
}

bool AppWindow::DrawMinimizeButton() {
  // Mirrors where ImGui places the close button, one button further left.
  const ImGuiStyle& style = ImGui::GetStyle();
  const float button_size = ImGui::GetFontSize();
  const ImVec2 window_pos = ImGui::GetWindowPos();
  const ImVec2 title_bar_max{window_pos.x + ImGui::GetWindowWidth(),
                             window_pos.y + ImGui::GetFrameHeight()};
  const ImVec2 button_min{title_bar_max.x - style.FramePadding.x -
                              (2.0F * button_size) - style.ItemInnerSpacing.x,
                          window_pos.y + style.FramePadding.y};
  const ImVec2 button_max = button_min + ImVec2(button_size, button_size);

  ImGui::PushClipRect(window_pos, title_bar_max, false);
  ImGui::SetCursorScreenPos(button_min);
  const bool clicked =
      ImGui::InvisibleButton("##minimize", ImVec2(button_size, button_size));

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  if (ImGui::IsItemHovered()) {
    draw_list->AddRectFilled(
        button_min, button_max,
        ImGui::GetColorU32(ImGui::IsItemActive() ? ImGuiCol_ButtonActive
                                                 : ImGuiCol_ButtonHovered),
        style.FrameRounding);
  }
  const float inset = button_size * 0.25F;
  const float line_y = button_max.y - inset;
  draw_list->AddLine(ImVec2(button_min.x + inset, line_y),
                     ImVec2(button_max.x - inset, line_y),
                     ImGui::GetColorU32(ImGuiCol_Text),
                     kMinimizeGlyphThickness);
  ImGui::PopClipRect();
  return clicked;
}

}  // namespace csopesy::apps
