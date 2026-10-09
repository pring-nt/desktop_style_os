#include "apps/task_manager.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <compare>
#include <cstddef>
#include <format>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "imgui.h"

#include "apps/sort_direction.h"
#include "core/imgui_flags.h"
#include "core/theme.h"
#include "data/dummy_process_table.h"

namespace csopesy::apps {

namespace {

using core::Theme;
using data::ProcessGroup;
using data::ProcessRow;
using data::ProcessStatus;

constexpr ImGuiTableFlags kTableFlags =
    core::CombineFlags(ImGuiTableFlags_RowBg, ImGuiTableFlags_BordersInnerV,
                       ImGuiTableFlags_ScrollY, ImGuiTableFlags_Resizable,
                       ImGuiTableFlags_SizingFixedFit);
constexpr ImGuiSelectableFlags kRowSelectFlags =
    core::CombineFlags(ImGuiSelectableFlags_SpanAllColumns);

constexpr int kNameColumn = 0;
constexpr int kStatusColumn = 1;
constexpr int kCpuColumn = 2;
constexpr int kMemoryColumn = 3;
constexpr int kColumnCount = 4;
constexpr const char* kEndTaskLabel = "End task";
constexpr float kHalf = 0.5F;

struct ColumnInfo {
  ProcessColumn column;
  const char* name;
  bool right_aligned;
};

// In table order: entry i describes table column i.
constexpr std::array kColumns{
    ColumnInfo{
        .column = ProcessColumn::kName,
        .name = "Name",
        .right_aligned = false,
    },
    ColumnInfo{
        .column = ProcessColumn::kStatus,
        .name = "Status",
        .right_aligned = false,
    },
    ColumnInfo{
        .column = ProcessColumn::kCpu,
        .name = "CPU",
        .right_aligned = true,
    },
    ColumnInfo{
        .column = ProcessColumn::kMemory,
        .name = "Memory",
        .right_aligned = true,
    },
};
static_assert(kColumns.size() == kColumnCount);

struct HeaderLabel {
  std::string total;
  const char* name;
  bool right_aligned;
  // Set on the sorted column.
  std::optional<SortDirection> arrow;
};

std::partial_ordering CompareBy(const ProcessRow& first,
                                const ProcessRow& second,
                                ProcessColumn column) {
  switch (column) {
    case ProcessColumn::kName:
      return first.name <=> second.name;
    case ProcessColumn::kStatus:
      return data::StatusLabel(first.status) <=>
             data::StatusLabel(second.status);
    case ProcessColumn::kCpu:
      return first.cpu_percent <=> second.cpu_percent;
    case ProcessColumn::kMemory:
      return first.memory_mb <=> second.memory_mb;
  }
  return std::partial_ordering::equivalent;
}

// Fills the current table cell with the Windows-style usage shading.
void ShadeCell(float heat) {
  ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                         ImGui::GetColorU32(HeatColor(heat)));
}

// Text that hugs the right edge of the current table cell.
void TextRightAligned(const std::string& text) {
  const float width = ImGui::CalcTextSize(text.c_str()).x;
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       ImGui::GetContentRegionAvail().x - width);
  ImGui::TextUnformatted(text.c_str());
}

// A small triangle on the far side of the last item from its text, pointing
// up for ascending and down for descending.
void DrawSortArrow(SortDirection direction, bool at_left) {
  const ImVec2 min = ImGui::GetItemRectMin();
  const ImVec2 max = ImGui::GetItemRectMax();
  const float center_x =
      at_left ? min.x + Theme::kSortArrowInset + Theme::kSortArrowHalfWidth
              : max.x - Theme::kSortArrowInset - Theme::kSortArrowHalfWidth;
  const ImVec2 center{center_x, (min.y + max.y) * kHalf};
  const float tip = direction == SortDirection::kAscending
                        ? -Theme::kSortArrowHalfHeight
                        : Theme::kSortArrowHalfHeight;
  ImGui::GetWindowDrawList()->AddTriangleFilled(
      center + ImVec2(0.0F, tip),
      center + ImVec2(-Theme::kSortArrowHalfWidth, -tip),
      center + ImVec2(Theme::kSortArrowHalfWidth, -tip),
      ImGui::GetColorU32(ImGuiCol_Text));
}

// A header cell in the Windows style: the column total on top (blank for
// columns without one), the clickable column name below. Returns true when
// the name was clicked.
bool HeaderCell(const HeaderLabel& label) {
  if (label.right_aligned) {
    TextRightAligned(label.total);
  } else {
    ImGui::TextUnformatted(label.total.c_str());
  }
  ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
                      ImVec2(label.right_aligned ? 1.0F : 0.0F, 0.0F));
  const bool clicked = ImGui::Selectable(label.name);
  ImGui::PopStyleVar();
  if (label.arrow) {
    DrawSortArrow(*label.arrow, label.right_aligned);
  }
  return clicked;
}

}  // namespace

float UsageHeat(double value, double full_scale) {
  if (full_scale <= 0.0) {
    return 0.0F;
  }
  return static_cast<float>(std::clamp(value / full_scale, 0.0, 1.0));
}

ImVec4 HeatColor(float heat) {
  const float t = std::clamp(heat, 0.0F, 1.0F);
  const ImVec4& low = Theme::kUsageHeatLowColor;
  const ImVec4& high = Theme::kUsageHeatHighColor;
  return {
      low.x + ((high.x - low.x) * t),
      low.y + ((high.y - low.y) * t),
      low.z + ((high.z - low.z) * t),
      low.w + ((high.w - low.w) * t),
  };
}

std::string FormatCpu(double percent) {
  return std::format("{:.1f}%", percent);
}

std::string FormatMemory(double megabytes) {
  return std::format("{:.1f} MB", megabytes);
}

std::string FormatTotal(double percent) {
  return std::format("{:.0f}%", std::round(percent));
}

std::string FormatGroupHeader(ProcessGroup group, std::size_t count) {
  const char* label =
      group == ProcessGroup::kApp ? "Apps" : "Background processes";
  return std::format("{} ({})", label, count);
}

ProcessSort ToggleSort(const ProcessSort& current, ProcessColumn clicked) {
  if (current.column == clicked) {
    return {
        .column = clicked,
        .direction = current.direction == SortDirection::kAscending
                         ? SortDirection::kDescending
                         : SortDirection::kAscending,
    };
  }
  const bool numeric =
      clicked == ProcessColumn::kCpu || clicked == ProcessColumn::kMemory;
  return {
      .column = clicked,
      .direction =
          numeric ? SortDirection::kDescending : SortDirection::kAscending,
  };
}

std::vector<const ProcessRow*> RowsInGroup(std::span<const ProcessRow> rows,
                                           ProcessGroup group,
                                           const ProcessSort& sort) {
  std::vector<const ProcessRow*> result;
  result.reserve(rows.size());
  for (const ProcessRow& row : rows) {
    if (row.group == group) {
      result.push_back(&row);
    }
  }
  if (!sort.column) {
    return result;
  }
  const ProcessColumn column = *sort.column;
  const bool descending = sort.direction == SortDirection::kDescending;
  std::ranges::stable_sort(
      result,
      [column, descending](const ProcessRow* first, const ProcessRow* second) {
        const std::partial_ordering order = CompareBy(*first, *second, column);
        if (order != 0) {
          return descending ? order > 0 : order < 0;
        }
        return first->name < second->name;
      });
  return result;
}

void TaskManager::EndSelectedTask() {
  if (!selected_process_.empty()) {
    table_->EndProcess(selected_process_);
    selected_process_ = {};
  }
}

void TaskManager::Draw() {
  if (!ImGui::BeginTabBar("##task_manager_tabs")) {
    return;
  }
  if (ImGui::BeginTabItem("Processes")) {
    DrawToolbar();
    DrawProcessTable();
    ImGui::EndTabItem();
  }
  ImGui::EndTabBar();
}

void TaskManager::DrawToolbar() {
  const float width = ImGui::CalcTextSize(kEndTaskLabel).x +
                      (ImGui::GetStyle().FramePadding.x * 2.0F);
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       ImGui::GetContentRegionAvail().x - width);
  ImGui::BeginDisabled(selected_process_.empty());
  if (ImGui::Button(kEndTaskLabel)) {
    EndSelectedTask();
  }
  ImGui::EndDisabled();
}

void TaskManager::DrawProcessTable() {
  if (!ImGui::BeginTable("##processes", kColumnCount, kTableFlags,
                         ImGui::GetContentRegionAvail())) {
    return;
  }
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
  ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed,
                          Theme::kStatusColumnWidth);
  ImGui::TableSetupColumn("CPU", ImGuiTableColumnFlags_WidthFixed,
                          Theme::kNumberColumnWidth);
  ImGui::TableSetupColumn("Memory", ImGuiTableColumnFlags_WidthFixed,
                          Theme::kNumberColumnWidth);

  DrawHeaderRow();
  DrawGroup(ProcessGroup::kApp);
  DrawGroup(ProcessGroup::kBackground);
  ImGui::EndTable();
}

void TaskManager::DrawHeaderRow() {
  const data::ProcessTotals totals = table_->totals();
  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                         ImGui::GetColorU32(ImGuiCol_TableHeaderBg));
  for (int index = 0; index < kColumnCount; ++index) {
    const ColumnInfo& info = kColumns.at(static_cast<std::size_t>(index));
    ImGui::TableSetColumnIndex(index);
    HeaderLabel label{
        .total = "",
        .name = info.name,
        .right_aligned = info.right_aligned,
        .arrow = std::nullopt,
    };
    if (info.column == ProcessColumn::kCpu) {
      ShadeCell(UsageHeat(totals.cpu_percent, data::kMaxPercent));
      label.total = FormatTotal(totals.cpu_percent);
    } else if (info.column == ProcessColumn::kMemory) {
      ShadeCell(UsageHeat(totals.memory_percent, data::kMaxPercent));
      label.total = FormatTotal(totals.memory_percent);
    }
    if (sort_.column == info.column) {
      label.arrow = sort_.direction;
    }
    if (HeaderCell(label)) {
      sort_ = ToggleSort(sort_, info.column);
    }
  }
}

void TaskManager::DrawGroup(ProcessGroup group) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(kNameColumn);
  ImGui::TextColored(
      Theme::kTaskManagerGroupColor, "%s",
      FormatGroupHeader(group, table_->CountInGroup(group)).c_str());

  for (const ProcessRow* row_pointer :
       RowsInGroup(table_->rows(), group, sort_)) {
    const ProcessRow& row = *row_pointer;
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(kNameColumn);
    ImGui::Indent(Theme::kTaskManagerRowIndent);
    const std::string name(row.name);
    if (ImGui::Selectable(name.c_str(), selected_process_ == row.name,
                          kRowSelectFlags)) {
      selected_process_ = row.name;
    }
    ImGui::Unindent(Theme::kTaskManagerRowIndent);

    ImGui::TableSetColumnIndex(kStatusColumn);
    const std::string status(data::StatusLabel(row.status));
    if (row.status == ProcessStatus::kSuspended) {
      ImGui::TextDisabled("%s", status.c_str());
    } else {
      ImGui::TextUnformatted(status.c_str());
    }
    ImGui::TableSetColumnIndex(kCpuColumn);
    ShadeCell(UsageHeat(row.cpu_percent, kCpuFullHeatPercent));
    TextRightAligned(FormatCpu(row.cpu_percent));
    ImGui::TableSetColumnIndex(kMemoryColumn);
    ShadeCell(UsageHeat(row.memory_mb, kMemoryFullHeatMb));
    TextRightAligned(FormatMemory(row.memory_mb));
  }
}

}  // namespace csopesy::apps
