#include "apps/terminal.h"

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"
#include "core/clock.h"
#include "data/dummy_process_table.h"
#include "data/terminal_commands.h"

namespace csopesy::apps {
namespace {

using testing::HeadlessImGui;

constexpr WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 664.0F),
};

core::Clock FixedClock() {
  const auto fixed_time = std::chrono::system_clock::time_point{};
  return core::Clock([fixed_time] { return fixed_time; });
}

bool Contains(const std::vector<std::string>& lines, const std::string& line) {
  return std::ranges::find(lines, line) != lines.end();
}

void DrawFrame(Terminal& terminal) {
  HeadlessImGui::BeginFrame();
  terminal.Render(kArea);
  HeadlessImGui::EndFrame();
}

TEST_CASE("Terminal opens with the banner") {
  const core::Clock clock = FixedClock();
  const data::DummyProcessTable processes;
  const Terminal terminal(clock, processes);
  CHECK(terminal.scrollback() == data::BannerLines());
}

TEST_CASE("Terminal echoes the prompt and prints a command's output") {
  const core::Clock clock = FixedClock();
  const data::DummyProcessTable processes;
  Terminal terminal(clock, processes);
  terminal.Submit("ver");
  CHECK(Contains(terminal.scrollback(), R"(C:\CSOPESY>ver)"));
  CHECK(Contains(terminal.scrollback(), "CSOPESY OS v1.0"));
}

TEST_CASE("cls clears the Terminal's scrollback") {
  const core::Clock clock = FixedClock();
  const data::DummyProcessTable processes;
  Terminal terminal(clock, processes);
  terminal.Submit("ver");
  terminal.Submit("cls");
  CHECK(terminal.scrollback().empty());
}

TEST_CASE("exit closes only the Terminal window") {
  const core::Clock clock = FixedClock();
  const data::DummyProcessTable processes;
  Terminal terminal(clock, processes);
  terminal.Open();
  terminal.Submit("exit");
  CHECK_FALSE(terminal.is_open());
  CHECK(terminal.scrollback() == data::BannerLines());
}

TEST_CASE("Typing a command and pressing Enter runs it") {
  const HeadlessImGui imgui;
  const core::Clock clock = FixedClock();
  const data::DummyProcessTable processes;
  Terminal terminal(clock, processes);
  terminal.Open();
  DrawFrame(terminal);
  DrawFrame(terminal);

  ImGuiIO& io = ImGui::GetIO();
  io.AddInputCharactersUTF8("whoami");
  DrawFrame(terminal);
  io.AddKeyEvent(ImGuiKey_Enter, true);
  DrawFrame(terminal);
  io.AddKeyEvent(ImGuiKey_Enter, false);
  DrawFrame(terminal);

  CHECK(Contains(terminal.scrollback(), R"(C:\CSOPESY>whoami)"));
  CHECK(Contains(terminal.scrollback(), R"(csopesy\user)"));
}

TEST_CASE("Up arrow recalls the previous command into the input") {
  const HeadlessImGui imgui;
  const core::Clock clock = FixedClock();
  const data::DummyProcessTable processes;
  Terminal terminal(clock, processes);
  terminal.Open();
  terminal.Submit("ver");
  DrawFrame(terminal);
  DrawFrame(terminal);

  ImGuiIO& io = ImGui::GetIO();
  io.AddKeyEvent(ImGuiKey_UpArrow, true);
  DrawFrame(terminal);
  io.AddKeyEvent(ImGuiKey_UpArrow, false);
  DrawFrame(terminal);
  io.AddKeyEvent(ImGuiKey_Enter, true);
  DrawFrame(terminal);
  io.AddKeyEvent(ImGuiKey_Enter, false);
  DrawFrame(terminal);

  CHECK(std::ranges::count(terminal.scrollback(), R"(C:\CSOPESY>ver)") == 2);
}

}  // namespace
}  // namespace csopesy::apps
