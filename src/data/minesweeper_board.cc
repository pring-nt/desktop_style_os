#include "data/minesweeper_board.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <span>
#include <utility>
#include <vector>

namespace csopesy::data {

namespace {

// Neighbors are the up to eight cells at most this far away on each axis.
constexpr int kNeighborReach = 1;

bool IsNeighborOrSelf(CellPos first, CellPos second) {
  return std::abs(first.column - second.column) <= kNeighborReach &&
         std::abs(first.row - second.row) <= kNeighborReach;
}

}  // namespace

MinesweeperBoard::MinesweeperBoard(const MinesweeperConfig& config,
                                   std::uint64_t seed)
    : config_(config), random_(seed) {
  Reset(config);
}

void MinesweeperBoard::Reset(const MinesweeperConfig& config) {
  config_ = config;
  cells_.assign(static_cast<std::size_t>(config.columns) *
                    static_cast<std::size_t>(config.rows),
                MinesweeperCell{});
  state_ = GameState::kReady;
  mines_placed_ = false;
  flags_ = 0;
  revealed_ = 0;
  exploded_.reset();
}

void MinesweeperBoard::Reveal(CellPos pos) {
  if (!InBounds(pos) || state_ == GameState::kWon ||
      state_ == GameState::kLost) {
    return;
  }
  const MinesweeperCell& target = cell(pos);
  if (target.revealed || target.flagged) {
    return;
  }
  if (!mines_placed_) {
    PlaceRandomMines(pos);
  }
  state_ = GameState::kPlaying;
  if (cell(pos).mine) {
    Lose(pos);
    return;
  }
  FloodReveal(pos);
  CheckWin();
}

void MinesweeperBoard::ToggleFlag(CellPos pos) {
  if (!InBounds(pos) || state_ == GameState::kWon ||
      state_ == GameState::kLost) {
    return;
  }
  MinesweeperCell& target = mutable_cell(pos);
  if (target.revealed) {
    return;
  }
  target.flagged = !target.flagged;
  flags_ += target.flagged ? 1 : -1;
}

void MinesweeperBoard::Chord(CellPos pos) {
  if (!InBounds(pos) || state_ != GameState::kPlaying) {
    return;
  }
  const MinesweeperCell& center = cell(pos);
  if (!center.revealed || center.adjacent_mines == 0) {
    return;
  }
  const std::vector<CellPos> neighbors = Neighbors(pos);
  int flagged = 0;
  for (const CellPos neighbor : neighbors) {
    flagged += cell(neighbor).flagged ? 1 : 0;
  }
  if (flagged != center.adjacent_mines) {
    return;
  }
  for (const CellPos neighbor : neighbors) {
    const MinesweeperCell& next = cell(neighbor);
    if (next.flagged || next.revealed) {
      continue;
    }
    if (next.mine) {
      Lose(neighbor);
      return;
    }
    FloodReveal(neighbor);
  }
  CheckWin();
}

void MinesweeperBoard::PlaceMines(std::span<const CellPos> mines) {
  for (const CellPos mine : mines) {
    mutable_cell(mine).mine = true;
  }
  config_.mines = static_cast<int>(mines.size());
  mines_placed_ = true;
  CountAdjacentMines();
}

const MinesweeperCell& MinesweeperBoard::cell(CellPos pos) const {
  return cells_.at(Index(pos));
}

bool MinesweeperBoard::InBounds(CellPos pos) const {
  return pos.column >= 0 && pos.column < config_.columns && pos.row >= 0 &&
         pos.row < config_.rows;
}

std::size_t MinesweeperBoard::Index(CellPos pos) const {
  return (static_cast<std::size_t>(pos.row) *
          static_cast<std::size_t>(config_.columns)) +
         static_cast<std::size_t>(pos.column);
}

MinesweeperCell& MinesweeperBoard::mutable_cell(CellPos pos) {
  return cells_.at(Index(pos));
}

std::vector<CellPos> MinesweeperBoard::Neighbors(CellPos pos) const {
  std::vector<CellPos> neighbors;
  for (int row = pos.row - kNeighborReach; row <= pos.row + kNeighborReach;
       ++row) {
    for (int column = pos.column - kNeighborReach;
         column <= pos.column + kNeighborReach; ++column) {
      const CellPos neighbor{.column = column, .row = row};
      if (InBounds(neighbor) && (column != pos.column || row != pos.row)) {
        neighbors.push_back(neighbor);
      }
    }
  }
  return neighbors;
}

void MinesweeperBoard::PlaceRandomMines(CellPos safe) {
  std::vector<CellPos> candidates;
  candidates.reserve(cells_.size());
  for (int row = 0; row < config_.rows; ++row) {
    for (int column = 0; column < config_.columns; ++column) {
      const CellPos pos{.column = column, .row = row};
      if (!IsNeighborOrSelf(pos, safe)) {
        candidates.push_back(pos);
      }
    }
  }
  const auto mines = static_cast<std::size_t>(config_.mines);
  // A crowded custom board may need the safe cell's neighbors too.
  if (candidates.size() < mines) {
    for (const CellPos neighbor : Neighbors(safe)) {
      candidates.push_back(neighbor);
    }
  }
  // A partial Fisher-Yates shuffle: the first `mines` candidates are a
  // uniform random pick.
  for (std::size_t i = 0; i < mines && i < candidates.size(); ++i) {
    const std::size_t pick =
        i + static_cast<std::size_t>(random_.NextBelow(candidates.size() - i));
    std::swap(candidates.at(i), candidates.at(pick));
    mutable_cell(candidates.at(i)).mine = true;
  }
  mines_placed_ = true;
  CountAdjacentMines();
}

void MinesweeperBoard::CountAdjacentMines() {
  for (int row = 0; row < config_.rows; ++row) {
    for (int column = 0; column < config_.columns; ++column) {
      const CellPos pos{.column = column, .row = row};
      int count = 0;
      for (const CellPos neighbor : Neighbors(pos)) {
        count += cell(neighbor).mine ? 1 : 0;
      }
      mutable_cell(pos).adjacent_mines = count;
    }
  }
}

void MinesweeperBoard::FloodReveal(CellPos start) {
  std::vector<CellPos> pending{start};
  while (!pending.empty()) {
    const CellPos pos = pending.back();
    pending.pop_back();
    MinesweeperCell& current = mutable_cell(pos);
    if (current.revealed || current.flagged || current.mine) {
      continue;
    }
    current.revealed = true;
    ++revealed_;
    if (current.adjacent_mines == 0) {
      for (const CellPos neighbor : Neighbors(pos)) {
        pending.push_back(neighbor);
      }
    }
  }
}

void MinesweeperBoard::Lose(CellPos mine) {
  state_ = GameState::kLost;
  exploded_ = mine;
  mutable_cell(mine).revealed = true;
}

void MinesweeperBoard::CheckWin() {
  const int safe_cells = (config_.columns * config_.rows) - config_.mines;
  if (revealed_ < safe_cells) {
    return;
  }
  state_ = GameState::kWon;
  for (MinesweeperCell& each : cells_) {
    if (each.mine) {
      each.flagged = true;
    }
  }
  flags_ = config_.mines;
}

}  // namespace csopesy::data
