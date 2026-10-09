#ifndef CSOPESY_SRC_APPS_MINESWEEPER_H_
#define CSOPESY_SRC_APPS_MINESWEEPER_H_

#include <cstdint>
#include <string>
#include <string_view>

#include "imgui.h"

#include "apps/app_window.h"
#include "data/minesweeper_board.h"

namespace csopesy::apps {

// The counters' three-digit display: "010", "-03"; clamped to -99..999.
[[nodiscard]] std::string FormatCounter(int value);

// The face on the new-game button, as a kaomoji: worried while a cell is
// being pressed, happy on a win, knocked out on a loss.
[[nodiscard]] std::string_view FaceFor(data::GameState state, bool pressing);

// The board's size in pixels for a difficulty.
[[nodiscard]] ImVec2 BoardSize(const data::MinesweeperConfig& config);

// Classic Minesweeper: a difficulty picker, a status bar with the mines left,
// the face button and a timer, then the board. The window resizes itself to
// fit the board when the difficulty changes.
class Minesweeper : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{260.0F, 340.0F};

  // `seed` varies the boards between launches; tests pass a fixed one.
  explicit Minesweeper(std::uint64_t seed);

  // Starts a new game, resizing the window if the difficulty changed.
  void NewGame(const data::MinesweeperConfig& config);

  [[nodiscard]] const data::MinesweeperBoard& board() const { return board_; }
  [[nodiscard]] float elapsed_seconds() const { return elapsed_seconds_; }

 protected:
  void Draw() override;

 private:
  void FitWindowToBoard() const;
  void DrawToolbar();
  void DrawStatusBar();
  void DrawBoard();

  data::MinesweeperBoard board_;
  float elapsed_seconds_ = 0.0F;
  bool fit_pending_ = true;
  // Whether the mouse is held down on a cell, for the face.
  bool pressing_ = false;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_MINESWEEPER_H_
