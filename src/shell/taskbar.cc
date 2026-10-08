#include "shell/taskbar.h"

#include <array>
#include <cstddef>

#include "imgui.h"
#include "imgui_internal.h"

#include "apps/app_window.h"
#include "core/imgui_flags.h"
#include "core/state_machine.h"
#include "core/theme.h"
#include "shell/desktop.h"
#include "shell/window_manager.h"

namespace csopesy::shell {

namespace {

using core::Theme;

constexpr ImGuiWindowFlags kTaskbarFlags = core::CombineFlags(
    ImGuiWindowFlags_NoTitleBar, ImGuiWindowFlags_NoResize,
    ImGuiWindowFlags_NoMove, ImGuiWindowFlags_NoScrollbar,
    ImGuiWindowFlags_NoScrollWithMouse, ImGuiWindowFlags_NoSavedSettings,
    ImGuiWindowFlags_NoCollapse);

constexpr ImGuiWindowFlags kDialogFlags = core::CombineFlags(
    ImGuiWindowFlags_AlwaysAutoResize, ImGuiWindowFlags_NoMove,
    ImGuiWindowFlags_NoSavedSettings);

constexpr const char* kShutdownDialogId = "Shut Down##pwr";
constexpr int kTrayButtonCount = 3;
constexpr ImVec2 kCenterPivot{0.5F, 0.5F};

// Icon geometry, as offsets from the button's center, for a 44 px button.
constexpr ImVec2 kFolderTabMin{-14.0F, -11.0F};
constexpr ImVec2 kFolderTabMax{-2.0F, -5.0F};
constexpr ImVec2 kFolderBodyMin{-14.0F, -7.0F};
constexpr ImVec2 kFolderBodyMax{14.0F, 11.0F};
constexpr float kFolderRounding = 2.5F;

constexpr ImVec2 kTerminalFrameMin{-14.0F, -11.0F};
constexpr ImVec2 kTerminalFrameMax{14.0F, 11.0F};
constexpr float kTerminalRounding = 3.0F;
constexpr std::array<ImVec2, 3> kTerminalChevron{
    ImVec2(-9.0F, -5.0F),
    ImVec2(-4.0F, 0.0F),
    ImVec2(-9.0F, 5.0F),
};
constexpr ImVec2 kTerminalCursorStart{-1.0F, 5.0F};
constexpr ImVec2 kTerminalCursorEnd{8.0F, 5.0F};

constexpr std::array<ImVec2, 7> kActivityLine{
    ImVec2(-15.0F, 2.0F), ImVec2(-8.0F, 2.0F), ImVec2(-4.0F, -9.0F),
    ImVec2(1.0F, 10.0F),  ImVec2(5.0F, -3.0F), ImVec2(8.0F, 2.0F),
    ImVec2(15.0F, 2.0F),
};

constexpr float kIconStroke = 2.0F;

template <std::size_t N>
void DrawPolyline(ImDrawList& draw_list, ImVec2 center,
                  const std::array<ImVec2, N>& offsets, ImU32 color) {
  std::array<ImVec2, N> points{};
  for (std::size_t i = 0; i < N; ++i) {
    points.at(i) = center + offsets.at(i);
  }
  draw_list.AddPolyline(points.data(), static_cast<int>(N), color, kIconStroke);
}

void DrawFolder(ImDrawList& draw_list, ImVec2 center) {
  draw_list.AddRectFilled(center + kFolderTabMin, center + kFolderTabMax,
                          ImGui::GetColorU32(Theme::kFolderTabColor),
                          kFolderRounding);
  draw_list.AddRectFilled(center + kFolderBodyMin, center + kFolderBodyMax,
                          ImGui::GetColorU32(Theme::kFolderIconColor),
                          kFolderRounding);
}

void DrawTerminal(ImDrawList& draw_list, ImVec2 center) {
  const ImU32 color = ImGui::GetColorU32(Theme::kTerminalIconColor);
  draw_list.AddRectFilled(
      center + kTerminalFrameMin, center + kTerminalFrameMax,
      ImGui::GetColorU32(Theme::kTerminalIconBackground), kTerminalRounding);
  draw_list.AddRect(center + kTerminalFrameMin, center + kTerminalFrameMax,
                    color, kTerminalRounding);
  DrawPolyline(draw_list, center, kTerminalChevron, color);
  draw_list.AddLine(center + kTerminalCursorStart, center + kTerminalCursorEnd,
                    color, kIconStroke);
}

void DrawActivity(ImDrawList& draw_list, ImVec2 center) {
  DrawPolyline(draw_list, center, kActivityLine,
               ImGui::GetColorU32(Theme::kActivityIconColor));
}

}  // namespace

IndicatorState IndicatorFor(const WindowManager& window_manager,
                            const apps::AppWindow& window) {
  if (!WindowManager::IsRunning(window)) {
    return IndicatorState::kHidden;
  }
  return window_manager.IsActive(window) ? IndicatorState::kActive
                                         : IndicatorState::kRunning;
}

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

ScreenRect TaskbarButtonRect(const ScreenRect& taskbar, std::size_t index) {
  constexpr float kStep =
      Theme::kTaskbarButtonSize + Theme::kTaskbarButtonSpacing;
  const float height = taskbar.max.y - taskbar.min.y;
  const ImVec2 min{
      taskbar.min.x + Theme::kTaskbarPadding +
          (kStep * static_cast<float>(index)),
      taskbar.min.y + ((height - Theme::kTaskbarButtonSize) * 0.5F),
  };
  return {
      .min = min,
      .max = min + ImVec2(Theme::kTaskbarButtonSize, Theme::kTaskbarButtonSize),
  };
}

ScreenRect TrayButtonRect(const ScreenRect& taskbar, TrayButton button) {
  constexpr float kStep = Theme::kTrayButtonSize.x + Theme::kTrayButtonSpacing;
  constexpr float kTrayWidth = (kStep * static_cast<float>(kTrayButtonCount)) -
                               Theme::kTrayButtonSpacing;
  const float height = taskbar.max.y - taskbar.min.y;
  const ImVec2 min{
      taskbar.max.x - Theme::kTaskbarPadding - kTrayWidth +
          (kStep * static_cast<float>(button)),
      taskbar.min.y + ((height - Theme::kTrayButtonSize.y) * 0.5F),
  };
  return {.min = min, .max = min + Theme::kTrayButtonSize};
}

void Taskbar::Pin(apps::AppWindow& window, TaskbarIcon icon) {
  buttons_.push_back({.window = &window, .icon = icon});
}

void Taskbar::Draw(WindowManager& window_manager,
                   core::StateMachine& state_machine) const {
  const ScreenRect bar = TaskbarRect(MainViewportRect());
  ImGui::SetNextWindowPos(bar.min);
  ImGui::SetNextWindowSize(bar.max - bar.min);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::kTaskbarColor);
  ImGui::Begin("##taskbar", nullptr, kTaskbarFlags);
  // App windows come to the front when focused; the taskbar must stay above
  // them, so it is moved back to the front every frame. A modal dialog stays
  // above (and dims) the taskbar instead.
  if (ImGui::GetTopMostPopupModal() == nullptr) {
    ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
  }
  ImDrawList& draw_list = *ImGui::GetWindowDrawList();
  draw_list.AddLine(bar.min, ImVec2(bar.max.x, bar.min.y),
                    ImGui::GetColorU32(Theme::kTaskbarBorderColor),
                    Theme::kTaskbarBorderThickness);

  for (std::size_t i = 0; i < buttons_.size(); ++i) {
    const AppButton& button = buttons_.at(i);
    const ScreenRect rect = TaskbarButtonRect(bar, i);
    ImGui::SetCursorScreenPos(rect.min);
    ImGui::PushID(button.window);
    const bool clicked = ImGui::InvisibleButton("##app", rect.max - rect.min);
    ImGui::PopID();
    const IndicatorState indicator =
        IndicatorFor(window_manager, *button.window);
    if (ImGui::IsItemHovered()) {
      draw_list.AddRectFilled(
          rect.min, rect.max,
          ImGui::GetColorU32(Theme::kTaskbarButtonHoverColor),
          Theme::kTaskbarButtonRounding);
    } else if (indicator == IndicatorState::kActive) {
      draw_list.AddRectFilled(
          rect.min, rect.max,
          ImGui::GetColorU32(Theme::kTaskbarButtonActiveColor),
          Theme::kTaskbarButtonRounding);
    }
    DrawIcon(draw_list, button.icon, rect);
    DrawIndicator(draw_list, indicator, rect);
    if (clicked) {
      window_manager.ToggleFromTaskbar(*button.window);
    }
  }

  DrawTray(bar, state_machine);
  DrawShutdownDialog(state_machine);
  ImGui::End();
  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);
}

void Taskbar::DrawIcon(ImDrawList& draw_list, TaskbarIcon icon,
                       const ScreenRect& button) {
  const ImVec2 center = (button.min + button.max) * 0.5F;
  switch (icon) {
    case TaskbarIcon::kFolder:
      DrawFolder(draw_list, center);
      break;
    case TaskbarIcon::kTerminal:
      DrawTerminal(draw_list, center);
      break;
    case TaskbarIcon::kActivity:
      DrawActivity(draw_list, center);
      break;
  }
}

void Taskbar::DrawIndicator(ImDrawList& draw_list, IndicatorState state,
                            const ScreenRect& button) {
  if (state == IndicatorState::kHidden) {
    return;
  }
  const bool active = state == IndicatorState::kActive;
  const float width =
      active ? Theme::kIndicatorActiveWidth : Theme::kIndicatorRunningWidth;
  const ImVec4& color =
      active ? Theme::kIndicatorActiveColor : Theme::kIndicatorRunningColor;
  const float center_x = (button.min.x + button.max.x) * 0.5F;
  const ImVec2 min{center_x - (width * 0.5F), button.max.y};
  const ImVec2 max{center_x + (width * 0.5F),
                   button.max.y + Theme::kIndicatorHeight};
  draw_list.AddRectFilled(min, max, ImGui::GetColorU32(color),
                          Theme::kIndicatorRounding);
}

void Taskbar::DrawTray(const ScreenRect& bar,
                       core::StateMachine& state_machine) {
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0F, 0.0F, 0.0F, 0.0F));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        Theme::kTaskbarButtonHoverColor);
  ImGui::PushStyleColor(ImGuiCol_Text, Theme::kTrayTextColor);
  // VOL and NET are placeholders until their popups arrive in Phase 4.
  ImGui::SetCursorScreenPos(TrayButtonRect(bar, TrayButton::kVolume).min);
  ImGui::Button("VOL", Theme::kTrayButtonSize);
  ImGui::SetCursorScreenPos(TrayButtonRect(bar, TrayButton::kNetwork).min);
  ImGui::Button("NET", Theme::kTrayButtonSize);

  ImGui::PushStyleColor(ImGuiCol_Text, Theme::kPowerTextColor);
  ImGui::SetCursorScreenPos(TrayButtonRect(bar, TrayButton::kPower).min);
  if (ImGui::Button("PWR", Theme::kTrayButtonSize)) {
    state_machine.RequestShutdown();
  }
  ImGui::PopStyleColor(4);
}

void Taskbar::DrawShutdownDialog(core::StateMachine& state_machine) {
  if (state_machine.is_shutdown_pending() &&
      !ImGui::IsPopupOpen(kShutdownDialogId)) {
    ImGui::OpenPopup(kShutdownDialogId);
  }
  ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                          ImGuiCond_Always, kCenterPivot);
  if (!ImGui::BeginPopupModal(kShutdownDialogId, nullptr, kDialogFlags)) {
    return;
  }
  ImGui::TextUnformatted("Shut down CSOPESY OS?");
  ImGui::Spacing();
  if (ImGui::Button("Shut Down", Theme::kDialogButtonSize)) {
    state_machine.ConfirmShutdown();
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Cancel", Theme::kDialogButtonSize) ||
      ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
    state_machine.CancelShutdown();
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

}  // namespace csopesy::shell
