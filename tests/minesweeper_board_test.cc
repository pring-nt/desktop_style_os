#include "data/minesweeper_board.h"

#include <array>
#include <cstdint>
#include <vector>

#include <doctest/doctest.h>

namespace csopesy::data {
namespace {

constexpr std::uint64_t kSeed = 12;
constexpr std::uint64_t kSeedCount = 25;
constexpr CellPos kCenter{.column = 4, .row = 4};
constexpr CellPos kCorner{.column = 0, .row = 0};
constexpr CellPos kFarCorner{.column = 8, .row = 8};
constexpr CellPos kNextToCorner{.column = 1, .row = 1};
constexpr CellPos kBesideWall{.column = 3, .row = 1};
constexpr CellPos kWallMine{.column = 2, .row = 1};
constexpr int kWallMines = 5;
constexpr int kMinesBesideWall = 3;

// Five mines walling off the four cells in the top-left corner.
constexpr std::array kWall{
    CellPos{.column = 2, .row = 0}, CellPos{.column = 2, .row = 1},
    CellPos{.column = 2, .row = 2}, CellPos{.column = 1, .row = 2},
    CellPos{.column = 0, .row = 2},
};
constexpr std::array kCornerMine{kCorner};

int CountMines(const MinesweeperBoard& board) {
  int mines = 0;
  for (int row = 0; row < board.config().rows; ++row) {
    for (int column = 0; column < board.config().columns; ++column) {
      mines += board.cell({.column = column, .row = row}).mine ? 1 : 0;
    }
  }
  return mines;
}

bool NeighborhoodIsMineFree(const MinesweeperBoard& board, CellPos center) {
  bool mine_free = true;
  for (int row = center.row - 1; row <= center.row + 1; ++row) {
    for (int column = center.column - 1; column <= center.column + 1;
         ++column) {
      mine_free = mine_free && !board.cell({.column = column, .row = row}).mine;
    }
  }
  return mine_free;
}

std::vector<bool> MineLayout(const MinesweeperBoard& board) {
  std::vector<bool> layout;
  for (int row = 0; row < board.config().rows; ++row) {
    for (int column = 0; column < board.config().columns; ++column) {
      layout.push_back(board.cell({.column = column, .row = row}).mine);
    }
  }
  return layout;
}

MinesweeperBoard WalledBoard() {
  MinesweeperBoard board(kBeginner, kSeed);
  board.PlaceMines(kWall);
  return board;
}

TEST_CASE("The first reveal is never a mine and places every mine") {
  bool all_safe = true;
  bool all_counted = true;
  for (std::uint64_t seed = 0; seed < kSeedCount; ++seed) {
    MinesweeperBoard board(kBeginner, seed);
    board.Reveal(kCenter);
    all_safe = all_safe && board.state() != GameState::kLost &&
               NeighborhoodIsMineFree(board, kCenter);
    all_counted = all_counted && CountMines(board) == kBeginner.mines;
  }
  CHECK(all_safe);
  CHECK(all_counted);
}

TEST_CASE("The same seed places the same mines") {
  MinesweeperBoard first(kBeginner, kSeed);
  MinesweeperBoard second(kBeginner, kSeed);
  first.Reveal(kCenter);
  second.Reveal(kCenter);
  CHECK(MineLayout(first) == MineLayout(second));
}

TEST_CASE("Cells count their neighboring mines") {
  const MinesweeperBoard board = WalledBoard();
  CHECK(board.cell(kBesideWall).adjacent_mines == kMinesBesideWall);
  CHECK(board.cell(kCorner).adjacent_mines == 0);
}

TEST_CASE("An empty cell flood-reveals up to the numbered border") {
  MinesweeperBoard board = WalledBoard();
  board.Reveal(kFarCorner);
  CHECK(board.state() == GameState::kPlaying);
  CHECK(board.cell(kFarCorner).revealed);
  CHECK(board.cell(kBesideWall).revealed);
  CHECK_FALSE(board.cell(kCorner).revealed);
}

TEST_CASE("Flags block reveals and count against the mines left") {
  MinesweeperBoard board = WalledBoard();
  board.ToggleFlag(kFarCorner);
  board.Reveal(kFarCorner);
  CHECK_FALSE(board.cell(kFarCorner).revealed);
  CHECK(board.mines_left() == kWallMines - 1);
  board.ToggleFlag(kFarCorner);
  CHECK(board.mines_left() == kWallMines);
}

TEST_CASE("Revealing a mine loses and ends the game") {
  MinesweeperBoard board = WalledBoard();
  board.Reveal(kWallMine);
  CHECK(board.state() == GameState::kLost);
  const CellPos exploded = board.exploded().value_or(kFarCorner);
  CHECK(exploded.column == kWallMine.column);
  CHECK(exploded.row == kWallMine.row);
  board.Reveal(kFarCorner);
  CHECK_FALSE(board.cell(kFarCorner).revealed);
}

TEST_CASE("Revealing every safe cell wins and flags the mines") {
  MinesweeperBoard board = WalledBoard();
  board.Reveal(kFarCorner);
  board.Reveal(kCorner);
  CHECK(board.state() == GameState::kWon);
  CHECK(board.cell(kWallMine).flagged);
  CHECK(board.mines_left() == 0);
}

TEST_CASE("Chording needs the right number of flags") {
  MinesweeperBoard board(kBeginner, kSeed);
  board.PlaceMines(kCornerMine);
  board.Reveal(kNextToCorner);
  board.Chord(kNextToCorner);
  CHECK_FALSE(board.cell(kFarCorner).revealed);
  board.ToggleFlag(kCorner);
  board.Chord(kNextToCorner);
  CHECK(board.state() == GameState::kWon);
}

TEST_CASE("Reset starts a fresh board at the new size") {
  MinesweeperBoard board = WalledBoard();
  board.Reveal(kWallMine);
  board.Reset(kExpert);
  CHECK(board.state() == GameState::kReady);
  CHECK(board.mines_left() == kExpert.mines);
  CHECK(board.InBounds({.column = kExpert.columns - 1, .row = 0}));
  CHECK_FALSE(board.InBounds({.column = kExpert.columns, .row = 0}));
}

}  // namespace
}  // namespace csopesy::data
