#include "apps/terminal.h"

#include <cstddef>
#include <format>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>

#include "imgui.h"

#include "apps/app_window.h"
#include "core/clock.h"
#include "core/imgui_flags.h"
#include "core/theme.h"
#include "data/dummy_process_table.h"
#include "data/terminal_commands.h"

namespace csopesy::apps {

namespace {

using core::Theme;

constexpr ImGuiInputTextFlags kInputFlags = core::CombineFlags(
    ImGuiInputTextFlags_EnterReturnsTrue, ImGuiInputTextFlags_CallbackHistory);
constexpr int kStyleColorCount = 4;

}  // namespace

Terminal::Terminal(const core::Clock& clock,
                   const data::DummyProcessTable& processes)
    : AppWindow("Terminal", kDefaultSize),
      clock_(&clock),
      processes_(&processes),
      scrollback_(data::BannerLines()) {}

void Terminal::Submit(std::string_view input) {
  scrollback_.push_back(std::format("{}{}", data::kTerminalPrompt, input));
  history_.Add(input);
  const std::string date = clock_->FormattedDate();
  const std::string time = clock_->FormattedTime();
  data::CommandResult result =
      data::RunCommand(input, {
                                  .date = date,
                                  .time = time,
                                  .processes = processes_,
                              });
  switch (result.effect) {
    case data::CommandEffect::kNone:
      break;
    case data::CommandEffect::kClearScreen:
      scrollback_.clear();
      break;
    case data::CommandEffect::kCloseWindow:
      // The next session starts fresh, as a new cmd.exe window would.
      scrollback_ = data::BannerLines();
      Close();
      return;
  }
  if (!result.lines.empty()) {
    scrollback_.insert(scrollback_.end(),
                       std::make_move_iterator(result.lines.begin()),
                       std::make_move_iterator(result.lines.end()));
    scrollback_.emplace_back();
  }
  scroll_to_bottom_ = true;
}

void Terminal::Draw() {
  ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::kTerminalBackground);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, Theme::kTerminalBackground);
  ImGui::PushStyleColor(ImGuiCol_Text, Theme::kTerminalTextColor);
  ImGui::PushStyleColor(ImGuiCol_Border, Theme::kTerminalBackground);
  DrawScrollback();
  DrawInputLine();
  ImGui::PopStyleColor(kStyleColorCount);
}

void Terminal::DrawScrollback() {
  ImGui::BeginChild("##scrollback",
                    ImVec2(0.0F, -ImGui::GetFrameHeightWithSpacing()),
                    ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
  ImGuiListClipper clipper;
  clipper.Begin(static_cast<int>(scrollback_.size()));
  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
      ImGui::TextUnformatted(
          scrollback_.at(static_cast<std::size_t>(i)).c_str());
    }
  }
  if (scroll_to_bottom_) {
    ImGui::SetScrollHereY(1.0F);
    scroll_to_bottom_ = false;
  }
  ImGui::EndChild();
}

void Terminal::DrawInputLine() {
  const std::string prompt(data::kTerminalPrompt);
  ImGui::AlignTextToFramePadding();
  ImGui::TextUnformatted(prompt.c_str());
  ImGui::SameLine(0.0F, 0.0F);
  ImGui::SetNextItemWidth(-1.0F);
  if (focus_input_ || ImGui::IsWindowAppearing()) {
    ImGui::SetKeyboardFocusHere();
    focus_input_ = false;
  }
  if (ImGui::InputText("##command", input_.data(), input_.size(), kInputFlags,
                       &Terminal::HandleInputCallback, this)) {
    const std::string command(input_.data());
    input_.fill('\0');
    Submit(command);
    focus_input_ = true;
  }
}

int Terminal::HandleInputCallback(ImGuiInputTextCallbackData* data) {
  if (data->EventFlag != ImGuiInputTextFlags_CallbackHistory) {
    return 0;
  }
  Terminal& terminal = *static_cast<Terminal*>(data->UserData);
  const std::optional<std::string_view> entry =
      data->EventKey == ImGuiKey_UpArrow ? terminal.history_.Previous()
                                         : terminal.history_.Next();
  if (!entry) {
    return 0;
  }
  const std::string text(*entry);
  data->DeleteChars(0, data->BufTextLen);
  data->InsertChars(0, text.c_str());
  return 0;
}

}  // namespace csopesy::apps
