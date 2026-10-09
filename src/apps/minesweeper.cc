#include "apps/minesweeper.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>

#include "imgui.h"

#include "apps/app_window.h"
#include "core/theme.h"
#include "data/minesweeper_board.h"

namespace csopesy::apps {

namespace {

using core::Theme;
using data::CellPos;
using data::GameState;
using data::MinesweeperCell;
using data::MinesweeperConfig;

constexpr int kCounterMin = -99;
constexpr int kCounterMax = 999;
constexpr float kHalf = 0.5F;
constexpr float kDifficultyComboWidth = 140.0F;
// Title bar, toolbar and status bar, each one frame high, with item spacing
// between the three content rows.
constexpr float kFrameRowsAboveBoard = 3.0F;
constexpr float kGapsAboveBoard = 2.0F;

constexpr std::array kDifficulties{
    &data::kBeginner,
    &data::kIntermediate,
    &data::kExpert,
};

// Mine and flag shapes, as shares of the cell size from its center.
constexpr float kMineRadius = 0.26F;
constexpr float kMineSpike = 0.38F;
constexpr float kMineSpikeDiagonal = 0.28F;
constexpr float kMineHighlightOffset = 0.09F;
constexpr float kMineHighlightRadius = 0.07F;
constexpr float kFlagPoleX = 0.06F;
constexpr float kFlagTop = -0.32F;
constexpr float kFlagBottom = -0.02F;
constexpr float kFlagTipX = -0.26F;
constexpr float kFlagTipY = -0.17F;
constexpr float kFlagBaseY = 0.28F;
constexpr float kFlagBaseHalfWidth = 0.22F;
constexpr float kCrossHalf = 0.32F;
constexpr float kNumberScale = 0.75F;

struct CellRect {
  ImVec2 min;
  ImVec2 max;
};

ImVec2 Center(const CellRect& rect) { return (rect.min + rect.max) * kHalf; }
float Size(const CellRect& rect) { return rect.max.x - rect.min.x; }

ImU32 Color(const ImVec4& color) { return ImGui::GetColorU32(color); }

void DrawRaised(ImDrawList& draw_list, const CellRect& rect) {
  const float bevel = Theme::kMineBevel;
  draw_list.AddRectFilled(rect.min, rect.max,
                          Color(Theme::kMineBevelDarkColor));
  draw_list.AddRectFilled(rect.min, rect.max - ImVec2(bevel, bevel),
                          Color(Theme::kMineBevelLightColor));
  draw_list.AddRectFilled(rect.min + ImVec2(bevel, bevel),
                          rect.max - ImVec2(bevel, bevel),
                          Color(Theme::kMineHiddenColor));
}

void DrawFlat(ImDrawList& draw_list, const CellRect& rect, const ImVec4& fill) {
  draw_list.AddRectFilled(rect.min, rect.max, Color(fill));
  draw_list.AddRect(rect.min, rect.max + ImVec2(1.0F, 1.0F),
                    Color(Theme::kMineGridColor));
}

void DrawMine(ImDrawList& draw_list, const CellRect& rect) {
  const ImVec2 center = Center(rect);
  const float size = Size(rect);
  const ImU32 black = Color(Theme::kMineBlackColor);
  const float spike = size * kMineSpike;
  const float diagonal = size * kMineSpikeDiagonal;
  draw_list.AddLine(center - ImVec2(spike, 0.0F), center + ImVec2(spike, 0.0F),
                    black, Theme::kMineStroke);
  draw_list.AddLine(center - ImVec2(0.0F, spike), center + ImVec2(0.0F, spike),
                    black, Theme::kMineStroke);
  draw_list.AddLine(center - ImVec2(diagonal, diagonal),
                    center + ImVec2(diagonal, diagonal), black,
                    Theme::kMineStroke);
  draw_list.AddLine(center + ImVec2(-diagonal, diagonal),
                    center + ImVec2(diagonal, -diagonal), black,
                    Theme::kMineStroke);
  draw_list.AddCircleFilled(center, size * kMineRadius, black);
  const float offset = size * kMineHighlightOffset;
  draw_list.AddCircleFilled(center - ImVec2(offset, offset),
                            size * kMineHighlightRadius,
                            Color(Theme::kMineHighlightColor));
}

void DrawFlag(ImDrawList& draw_list, const CellRect& rect) {
  const ImVec2 center = Center(rect);
  const float size = Size(rect);
  const ImU32 black = Color(Theme::kMineBlackColor);
  const float pole_x = center.x + (size * kFlagPoleX);
  draw_list.AddTriangleFilled(
      ImVec2(pole_x, center.y + (size * kFlagTop)),
      ImVec2(pole_x, center.y + (size * kFlagBottom)),
      ImVec2(center.x + (size * kFlagTipX), center.y + (size * kFlagTipY)),
      Color(Theme::kMineFlagColor));
  draw_list.AddLine(ImVec2(pole_x, center.y + (size * kFlagTop)),
                    ImVec2(pole_x, center.y + (size * kFlagBaseY)), black,
                    Theme::kMineStroke);
  const float base_half = size * kFlagBaseHalfWidth;
  draw_list.AddLine(
      ImVec2(center.x - base_half, center.y + (size * kFlagBaseY)),
      ImVec2(center.x + base_half, center.y + (size * kFlagBaseY)), black,
      Theme::kMineStroke + Theme::kMineStroke);
}

void DrawCross(ImDrawList& draw_list, const CellRect& rect) {
  const ImVec2 center = Center(rect);
  const float half = Size(rect) * kCrossHalf;
  const ImU32 red = Color(Theme::kMineFlagColor);
  draw_list.AddLine(center - ImVec2(half, half), center + ImVec2(half, half),
                    red, Theme::kMineStroke);
  draw_list.AddLine(center + ImVec2(-half, half), center + ImVec2(half, -half),
                    red, Theme::kMineStroke);
}

void DrawNumber(ImDrawList& draw_list, const CellRect& rect, int count) {
  const std::string text = std::to_string(count);
  const float font_size = Size(rect) * kNumberScale;
  const ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(
      font_size, Size(rect), 0.0F, text.c_str());
  const ImVec4& color =
      Theme::kMineNumberColors.at(static_cast<std::size_t>(count - 1));
  draw_list.AddText(ImGui::GetFont(), font_size,
                    Center(rect) - (text_size * kHalf), Color(color),
                    text.c_str());
}

struct CellView {
  const MinesweeperCell* cell;
  GameState state;
  bool exploded;
  bool pressed;
};

void DrawCell(ImDrawList& draw_list, const CellRect& rect,
              const CellView& view) {
  const MinesweeperCell& cell = *view.cell;
  const bool lost = view.state == GameState::kLost;
  if (view.exploded) {
    DrawFlat(draw_list, rect, Theme::kMineExplodedColor);
    DrawMine(draw_list, rect);
    return;
  }
  if (cell.revealed) {
    DrawFlat(draw_list, rect, Theme::kMineRevealedColor);
    if (cell.adjacent_mines > 0) {
      DrawNumber(draw_list, rect, cell.adjacent_mines);
    }
    return;
  }
  if (lost && cell.mine && !cell.flagged) {
    DrawFlat(draw_list, rect, Theme::kMineRevealedColor);
    DrawMine(draw_list, rect);
    return;
  }
  if (lost && cell.flagged && !cell.mine) {
    DrawFlat(draw_list, rect, Theme::kMineRevealedColor);
    DrawMine(draw_list, rect);
    DrawCross(draw_list, rect);
    return;
  }
  if (view.pressed && !cell.flagged) {
    DrawFlat(draw_list, rect, Theme::kMineRevealedColor);
    return;
  }
  DrawRaised(draw_list, rect);
  if (cell.flagged) {
    DrawFlag(draw_list, rect);
  }
}

// A black box with red digits, like the classic seven-segment counters.
void DrawCounter(const std::string& text) {
  const ImVec2 padding = ImGui::GetStyle().FramePadding;
  const ImVec2 size = ImGui::CalcTextSize(text.c_str()) + padding + padding;
  const ImVec2 min = ImGui::GetCursorScreenPos();
  ImDrawList& draw_list = *ImGui::GetWindowDrawList();
  draw_list.AddRectFilled(min, min + size, Color(Theme::kMineCounterBackground),
                          ImGui::GetStyle().FrameRounding);
  draw_list.AddText(min + padding, Color(Theme::kMineCounterColor),
                    text.c_str());
  ImGui::Dummy(size);
}

}  // namespace

std::string FormatCounter(int value) {
  return std::format("{:03}", std::clamp(value, kCounterMin, kCounterMax));
}

std::string_view FaceFor(GameState state, bool pressing) {
  switch (state) {
    case GameState::kWon:
      return "\\(^o^)/";
    case GameState::kLost:
      return "(x_x)";
    case GameState::kReady:
    case GameState::kPlaying:
      break;
  }
  return pressing ? "(o_o)" : "(^_^)";
}

ImVec2 BoardSize(const MinesweeperConfig& config) {
  return {static_cast<float>(config.columns) * Theme::kMineCellSize,
          static_cast<float>(config.rows) * Theme::kMineCellSize};
}

Minesweeper::Minesweeper(std::uint64_t seed)
    : AppWindow("Minesweeper", kDefaultSize), board_(data::kBeginner, seed) {}

void Minesweeper::NewGame(const MinesweeperConfig& config) {
  fit_pending_ = fit_pending_ || config.name != board_.config().name;
  board_.Reset(config);
  elapsed_seconds_ = 0.0F;
}

void Minesweeper::Draw() {
  if (fit_pending_) {
    FitWindowToBoard();
    fit_pending_ = false;
  }
  if (board_.state() == GameState::kPlaying) {
    elapsed_seconds_ += ImGui::GetIO().DeltaTime;
  }
  DrawToolbar();
  DrawStatusBar();
  DrawBoard();
}

void Minesweeper::FitWindowToBoard() const {
  const ImGuiStyle& style = ImGui::GetStyle();
  const ImVec2 board = BoardSize(board_.config());
  const ImVec2 padding = style.WindowPadding + style.WindowPadding;
  const float chrome = (ImGui::GetFrameHeight() * kFrameRowsAboveBoard) +
                       (style.ItemSpacing.y * kGapsAboveBoard) + padding.y;
  ImGui::SetWindowSize(ImVec2(board.x + padding.x, board.y + chrome));
}

void Minesweeper::DrawToolbar() {
  ImGui::SetNextItemWidth(kDifficultyComboWidth);
  const std::string preview(board_.config().name);
  if (!ImGui::BeginCombo("##difficulty", preview.c_str())) {
    return;
  }
  for (const MinesweeperConfig* config : kDifficulties) {
    const std::string name(config->name);
    if (ImGui::Selectable(name.c_str(), config->name == preview)) {
      NewGame(*config);
    }
  }
  ImGui::EndCombo();
}

void Minesweeper::DrawStatusBar() {
  const float row_start = ImGui::GetCursorPosX();
  const float width = ImGui::GetContentRegionAvail().x;
  DrawCounter(FormatCounter(board_.mines_left()));

  const std::string face =
      std::format("{}###face", FaceFor(board_.state(), pressing_));
  const float face_width = ImGui::CalcTextSize(face.c_str(), nullptr, true).x +
                           (ImGui::GetStyle().FramePadding.x * 2.0F);
  ImGui::SameLine(row_start + ((width - face_width) * kHalf));
  if (ImGui::Button(face.c_str())) {
    NewGame(board_.config());
  }

  const std::string time = FormatCounter(static_cast<int>(elapsed_seconds_));
  const float time_width = ImGui::CalcTextSize(time.c_str()).x +
                           (ImGui::GetStyle().FramePadding.x * 2.0F);
  ImGui::SameLine(row_start + width - time_width);
  DrawCounter(time);
}

void Minesweeper::DrawBoard() {
  const data::MinesweeperConfig& config = board_.config();
  const ImVec2 board = BoardSize(config);
  const ImVec2 origin =
      ImGui::GetCursorScreenPos() +
      ImVec2(
          std::max(0.0F, (ImGui::GetContentRegionAvail().x - board.x) * kHalf),
          0.0F);
  const ImVec2 cell_size{Theme::kMineCellSize, Theme::kMineCellSize};
  ImDrawList& draw_list = *ImGui::GetWindowDrawList();
  const std::optional<CellPos> exploded = board_.exploded();
  pressing_ = false;

  for (int row = 0; row < config.rows; ++row) {
    for (int column = 0; column < config.columns; ++column) {
      const CellPos pos{.column = column, .row = row};
      const CellRect rect{
          .min = origin + ImVec2(static_cast<float>(column) * cell_size.x,
                                 static_cast<float>(row) * cell_size.y),
          .max = origin + ImVec2(static_cast<float>(column + 1) * cell_size.x,
                                 static_cast<float>(row + 1) * cell_size.y),
      };
      ImGui::PushID((row * config.columns) + column);
      ImGui::SetCursorScreenPos(rect.min);
      const bool left_clicked = ImGui::InvisibleButton("##cell", cell_size);
      const bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);
      const bool pressed = ImGui::IsItemActive();
      ImGui::PopID();

      pressing_ = pressing_ || pressed;
      const MinesweeperCell& cell = board_.cell(pos);
      if (left_clicked) {
        if (cell.revealed) {
          board_.Chord(pos);
        } else {
          board_.Reveal(pos);
        }
      } else if (right_clicked) {
        board_.ToggleFlag(pos);
      }
      const bool over = board_.state() == GameState::kWon ||
                        board_.state() == GameState::kLost;
      DrawCell(
          draw_list, rect,
          CellView{
              .cell = &board_.cell(pos),
              .state = board_.state(),
              .exploded = exploded.has_value() && exploded->column == column &&
                          exploded->row == row,
              .pressed = pressed && !over,
          });
    }
  }
  ImGui::SetCursorScreenPos(origin);
  ImGui::Dummy(board);
}

}  // namespace csopesy::apps
