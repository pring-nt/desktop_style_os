#include "apps/task_manager.h"

#include <cmath>
#include <cstddef>
#include <format>
#include <string>

#include "imgui.h"

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

// Text that hugs the right edge of the current table cell.
void TextRightAligned(const std::string& text) {
  const float width = ImGui::CalcTextSize(text.c_str()).x;
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       ImGui::GetContentRegionAvail().x - width);
  ImGui::TextUnformatted(text.c_str());
}

// A header cell in the Windows style: the column total on top (blank for
// columns without one), the column name below.
void HeaderCell(const std::string& total, const char* name,
                bool right_aligned) {
  if (right_aligned) {
    TextRightAligned(total);
    TextRightAligned(name);
    return;
  }
  ImGui::TextUnformatted(total.c_str());
  ImGui::TextUnformatted(name);
}

}  // namespace

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

void TaskManager::Draw() {
  if (!ImGui::BeginTabBar("##task_manager_tabs")) {
    return;
  }
  if (ImGui::BeginTabItem("Processes")) {
    DrawProcessTable();
    ImGui::EndTabItem();
  }
  ImGui::EndTabBar();
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

  const data::ProcessTotals totals = table_->totals();
  ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
  ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                         ImGui::GetColorU32(ImGuiCol_TableHeaderBg));
  ImGui::TableSetColumnIndex(kNameColumn);
  HeaderCell("", "Name", false);
  ImGui::TableSetColumnIndex(kStatusColumn);
  HeaderCell("", "Status", false);
  ImGui::TableSetColumnIndex(kCpuColumn);
  HeaderCell(FormatTotal(totals.cpu_percent), "CPU", true);
  ImGui::TableSetColumnIndex(kMemoryColumn);
  HeaderCell(FormatTotal(totals.memory_percent), "Memory", true);

  DrawGroup(ProcessGroup::kApp);
  DrawGroup(ProcessGroup::kBackground);
  ImGui::EndTable();
}

void TaskManager::DrawGroup(ProcessGroup group) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(kNameColumn);
  ImGui::TextColored(
      Theme::kTaskManagerGroupColor, "%s",
      FormatGroupHeader(group, table_->CountInGroup(group)).c_str());

  for (const ProcessRow& row : table_->rows()) {
    if (row.group != group) {
      continue;
    }
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
    TextRightAligned(FormatCpu(row.cpu_percent));
    ImGui::TableSetColumnIndex(kMemoryColumn);
    TextRightAligned(FormatMemory(row.memory_mb));
  }
}

}  // namespace csopesy::apps
