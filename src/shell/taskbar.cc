#include "shell/taskbar.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "apps/app_window.h"
#include "core/imgui_flags.h"
#include "core/theme.h"
#include "shell/desktop.h"

namespace csopesy::shell {

namespace {

using core::Theme;

constexpr ImGuiWindowFlags kTaskbarFlags = core::CombineFlags(
    ImGuiWindowFlags_NoTitleBar, ImGuiWindowFlags_NoResize,
    ImGuiWindowFlags_NoMove, ImGuiWindowFlags_NoScrollbar,
    ImGuiWindowFlags_NoScrollWithMouse, ImGuiWindowFlags_NoSavedSettings,
    ImGuiWindowFlags_NoCollapse);

}  // namespace

ScreenRect TaskbarRect(const ScreenRect& viewport) {
  return {
      .min = ImVec2(viewport.min.x, viewport.max.y - Theme::kTaskbarHeight),
      .max = viewport.max,
  };
}

apps::WorkArea WorkAreaAboveTaskbar(const ScreenRect& viewport) {
  return {
      .min = viewport.min,
      .max = ImVec2(viewport.max.x, TaskbarRect(viewport).min.y),
  };
}

void Taskbar::Draw() {
  const ScreenRect bar = TaskbarRect(MainViewportRect());
  ImGui::SetNextWindowPos(bar.min);
  ImGui::SetNextWindowSize(bar.max - bar.min);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::kTaskbarColor);
  ImGui::Begin("##taskbar", nullptr, kTaskbarFlags);
  // App windows come to the front when focused; the taskbar must stay above
  // them, so it is moved back to the front every frame.
  ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
  ImGui::GetWindowDrawList()->AddLine(
      bar.min, ImVec2(bar.max.x, bar.min.y),
      ImGui::GetColorU32(Theme::kTaskbarBorderColor),
      Theme::kTaskbarBorderThickness);
  ImGui::End();
  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);
}

}  // namespace csopesy::shell
