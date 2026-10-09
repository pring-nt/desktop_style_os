#ifndef CSOPESY_SRC_DATA_MINESWEEPER_BOARD_H_
#define CSOPESY_SRC_DATA_MINESWEEPER_BOARD_H_

#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include "data/random.h"

namespace csopesy::data {

struct MinesweeperConfig {
  std::string_view name;
  int columns;
  int rows;
  int mines;
};

// The three Windows difficulties.
inline constexpr MinesweeperConfig kBeginner{
    .name = "Beginner",
    .columns = 9,
    .rows = 9,
    .mines = 10,
};
inline constexpr MinesweeperConfig kIntermediate{
    .name = "Intermediate",
    .columns = 16,
    .rows = 16,
    .mines = 40,
};
inline constexpr MinesweeperConfig kExpert{
    .name = "Expert",
    .columns = 30,
    .rows = 16,
    .mines = 99,
};

// kReady until the first reveal places the mines.
enum class GameState : std::uint8_t { kReady, kPlaying, kWon, kLost };

struct CellPos {
  int column;
  int row;
};

struct MinesweeperCell {
  bool mine = false;
  bool revealed = false;
  bool flagged = false;
  // Mines in the eight neighboring cells.
  int adjacent_mines = 0;
};

// The rules of Minesweeper, with no drawing. Mines are placed on the first
// reveal, away from the revealed cell and its neighbors, so the first click
// is always safe. Placement uses SplitMix64, so a seed always gives the same
// board.
class MinesweeperBoard {
 public:
  MinesweeperBoard(const MinesweeperConfig& config, std::uint64_t seed);

  // Starts a new game, keeping the random sequence going so each game
  // differs.
  void Reset(const MinesweeperConfig& config);

  // Reveals a hidden, unflagged cell. A cell with no adjacent mines reveals
  // its neighbors too (flood fill). Revealing a mine loses the game.
  void Reveal(CellPos pos);
  // Places or removes a flag on a hidden cell.
  void ToggleFlag(CellPos pos);
  // On a revealed number whose adjacent flags match it, reveals the other
  // neighbors.
  void Chord(CellPos pos);

  // Places mines on exactly these cells instead of at random, for tests.
  // Must be called before the first reveal.
  void PlaceMines(std::span<const CellPos> mines);

  [[nodiscard]] const MinesweeperConfig& config() const { return config_; }
  [[nodiscard]] GameState state() const { return state_; }
  [[nodiscard]] const MinesweeperCell& cell(CellPos pos) const;
  [[nodiscard]] bool InBounds(CellPos pos) const;
  // Mines minus flags, as the counter shows it; negative with too many
  // flags.
  [[nodiscard]] int mines_left() const { return config_.mines - flags_; }
  // The mine that was revealed to lose the game.
  [[nodiscard]] std::optional<CellPos> exploded() const { return exploded_; }

 private:
  [[nodiscard]] std::size_t Index(CellPos pos) const;
  MinesweeperCell& mutable_cell(CellPos pos);
  [[nodiscard]] std::vector<CellPos> Neighbors(CellPos pos) const;
  void PlaceRandomMines(CellPos safe);
  void CountAdjacentMines();
  void FloodReveal(CellPos start);
  void Lose(CellPos mine);
  void CheckWin();

  MinesweeperConfig config_;
  SplitMix64 random_;
  std::vector<MinesweeperCell> cells_;
  GameState state_ = GameState::kReady;
  bool mines_placed_ = false;
  int flags_ = 0;
  int revealed_ = 0;
  std::optional<CellPos> exploded_;
};

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_MINESWEEPER_BOARD_H_
