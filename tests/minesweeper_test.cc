#include "apps/minesweeper.h"

#include <cstdint>
#include <ostream>  // IWYU pragma: keep (doctest prints std::string_view)

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"
#include "core/theme.h"
#include "data/minesweeper_board.h"

namespace csopesy::apps {
namespace {

using data::GameState;
using testing::HeadlessImGui;

constexpr std::uint64_t kSeed = 12;
constexpr int kFrames = 3;
constexpr int kTen = 10;
constexpr int kMinusThree = -3;
constexpr int kTooHigh = 1500;
constexpr int kTooLow = -150;
constexpr WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 664.0F),
};

TEST_CASE("Counters show three digits, clamped") {
  CHECK(FormatCounter(kTen) == "010");
  CHECK(FormatCounter(kMinusThree) == "-03");
  CHECK(FormatCounter(kTooHigh) == "999");
  CHECK(FormatCounter(kTooLow) == "-99");
}

TEST_CASE("The face follows the game") {
  CHECK(FaceFor(GameState::kPlaying, false) == "(^_^)");
  CHECK(FaceFor(GameState::kPlaying, true) == "(o_o)");
  CHECK(FaceFor(GameState::kWon, false) == "\\(^o^)/");
  CHECK(FaceFor(GameState::kLost, true) == "(x_x)");
}

TEST_CASE("The board is one cell size per square") {
  const ImVec2 size = BoardSize(data::kExpert);
  CHECK(size.x ==
        static_cast<float>(data::kExpert.columns) * core::Theme::kMineCellSize);
  CHECK(size.y ==
        static_cast<float>(data::kExpert.rows) * core::Theme::kMineCellSize);
}

TEST_CASE("Minesweeper starts on Beginner and switches difficulty") {
  Minesweeper minesweeper(kSeed);
  CHECK(minesweeper.board().config().name == data::kBeginner.name);
  minesweeper.NewGame(data::kExpert);
  CHECK(minesweeper.board().config().name == data::kExpert.name);
  CHECK(minesweeper.board().state() == GameState::kReady);
  CHECK(minesweeper.elapsed_seconds() == 0.0F);
}

TEST_CASE("Minesweeper draws a full board headless") {
  const HeadlessImGui imgui;
  Minesweeper minesweeper(kSeed);
  minesweeper.Open();
  minesweeper.NewGame(data::kExpert);
  for (int frame = 0; frame < kFrames; ++frame) {
    HeadlessImGui::BeginFrame();
    minesweeper.Render(kArea);
    HeadlessImGui::EndFrame();
  }
  CHECK(minesweeper.is_open());
  CHECK(minesweeper.board().state() == GameState::kReady);
}

}  // namespace
}  // namespace csopesy::apps
