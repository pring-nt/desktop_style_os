#include "apps/file_explorer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "imgui.h"

#include "apps/sort_direction.h"
#include "core/imgui_flags.h"
#include "core/theme.h"

namespace csopesy::apps {

namespace {

using core::Theme;

constexpr ImGuiTableFlags kFileTableFlags = core::CombineFlags(
    ImGuiTableFlags_Sortable, ImGuiTableFlags_RowBg,
    ImGuiTableFlags_BordersInnerV, ImGuiTableFlags_ScrollY,
    ImGuiTableFlags_Resizable, ImGuiTableFlags_SizingFixedFit);
constexpr ImGuiSelectableFlags kRowSelectFlags =
    core::CombineFlags(ImGuiSelectableFlags_SpanAllColumns);
constexpr ImGuiTreeNodeFlags kRootNodeFlags = core::CombineFlags(
    ImGuiTreeNodeFlags_DefaultOpen, ImGuiTreeNodeFlags_OpenOnArrow,
    ImGuiTreeNodeFlags_SpanAvailWidth);
constexpr ImGuiTreeNodeFlags kLeafNodeFlags = core::CombineFlags(
    ImGuiTreeNodeFlags_Leaf, ImGuiTreeNodeFlags_NoTreePushOnOpen,
    ImGuiTreeNodeFlags_SpanAvailWidth);

constexpr int kNameColumn = 0;
constexpr int kTypeColumn = 1;
constexpr int kSizeColumn = 2;
constexpr int kModifiedColumn = 3;
constexpr int kColumnCount = 4;
constexpr std::size_t kDigitsPerGroup = 3;

constexpr std::array kChildFolders{
    Folder::kDocuments,
    Folder::kPictures,
    Folder::kDownloads,
    Folder::kSystem32,
};

constexpr std::array kThisPcEntries{
    FileEntry{
        .name = "Documents",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-10-02 09:15",
    },
    FileEntry{
        .name = "Pictures",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-09-30 18:40",
    },
    FileEntry{
        .name = "Downloads",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-10-07 21:03",
    },
    FileEntry{
        .name = "System32",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-08-14 07:22",
    },
};

constexpr std::array kDocumentsEntries{
    FileEntry{
        .name = "report.docx",
        .type = "Word Document",
        .size_kb = 245,
        .modified = "2026-09-28 14:12",
    },
    FileEntry{
        .name = "notes.txt",
        .type = "Text Document",
        .size_kb = 4,
        .modified = "2026-10-06 22:41",
    },
    FileEntry{
        .name = "budget.xlsx",
        .type = "Excel Worksheet",
        .size_kb = 88,
        .modified = "2026-09-15 10:05",
    },
    FileEntry{
        .name = "slides.pptx",
        .type = "PowerPoint Presentation",
        .size_kb = 1840,
        .modified = "2026-10-01 16:30",
    },
    FileEntry{
        .name = "resume.pdf",
        .type = "PDF Document",
        .size_kb = 312,
        .modified = "2026-07-21 11:48",
    },
    FileEntry{
        .name = "todo.txt",
        .type = "Text Document",
        .size_kb = 1,
        .modified = "2026-10-08 08:02",
    },
    FileEntry{
        .name = "thesis_draft.docx",
        .type = "Word Document",
        .size_kb = 1120,
        .modified = "2026-10-05 23:57",
    },
    FileEntry{
        .name = "meeting_minutes.txt",
        .type = "Text Document",
        .size_kb = 6,
        .modified = "2026-09-29 15:20",
    },
    FileEntry{
        .name = "schedule.xlsx",
        .type = "Excel Worksheet",
        .size_kb = 54,
        .modified = "2026-09-02 09:00",
    },
    FileEntry{
        .name = "readme.md",
        .type = "Markdown File",
        .size_kb = 3,
        .modified = "2026-08-30 13:37",
    },
    FileEntry{
        .name = "project_plan.pdf",
        .type = "PDF Document",
        .size_kb = 760,
        .modified = "2026-09-11 17:45",
    },
    FileEntry{
        .name = "letter.docx",
        .type = "Word Document",
        .size_kb = 38,
        .modified = "2026-06-18 12:10",
    },
};

constexpr std::array kPicturesEntries{
    FileEntry{
        .name = "Screenshots",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-10-04 19:12",
    },
    FileEntry{
        .name = "wallpaper.jpg",
        .type = "JPEG Image",
        .size_kb = 2048,
        .modified = "2026-08-01 20:00",
    },
    FileEntry{
        .name = "vacation_01.png",
        .type = "PNG Image",
        .size_kb = 3412,
        .modified = "2026-07-12 09:31",
    },
    FileEntry{
        .name = "vacation_02.png",
        .type = "PNG Image",
        .size_kb = 3378,
        .modified = "2026-07-12 09:33",
    },
    FileEntry{
        .name = "profile.jpg",
        .type = "JPEG Image",
        .size_kb = 186,
        .modified = "2026-05-03 14:25",
    },
    FileEntry{
        .name = "diagram.svg",
        .type = "SVG Document",
        .size_kb = 27,
        .modified = "2026-09-22 11:02",
    },
};

constexpr std::array kDownloadsEntries{
    FileEntry{
        .name = "setup.exe",
        .type = "Application",
        .size_kb = 48210,
        .modified = "2026-10-07 21:03",
    },
    FileEntry{
        .name = "music.zip",
        .type = "Compressed (zipped) Folder",
        .size_kb = 152304,
        .modified = "2026-09-19 18:47",
    },
    FileEntry{
        .name = "paper.pdf",
        .type = "PDF Document",
        .size_kb = 1904,
        .modified = "2026-10-03 10:16",
    },
    FileEntry{
        .name = "installer.msi",
        .type = "Windows Installer Package",
        .size_kb = 23552,
        .modified = "2026-08-27 16:09",
    },
    FileEntry{
        .name = "dataset.csv",
        .type = "CSV File",
        .size_kb = 5120,
        .modified = "2026-09-25 13:58",
    },
};

constexpr std::array kSystem32Entries{
    FileEntry{
        .name = "drivers",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-08-14 07:22",
    },
    FileEntry{
        .name = "config",
        .type = kFolderType,
        .size_kb = 0,
        .modified = "2026-08-14 07:22",
    },
    FileEntry{
        .name = "kernel32.dll",
        .type = "Application extension",
        .size_kb = 812,
        .modified = "2026-08-14 07:20",
    },
    FileEntry{
        .name = "user32.dll",
        .type = "Application extension",
        .size_kb = 1664,
        .modified = "2026-08-14 07:20",
    },
    FileEntry{
        .name = "hal.dll",
        .type = "Application extension",
        .size_kb = 96,
        .modified = "2026-08-14 07:19",
    },
    FileEntry{
        .name = "cmd.exe",
        .type = "Application",
        .size_kb = 332,
        .modified = "2026-08-14 07:21",
    },
    FileEntry{
        .name = "notepad.exe",
        .type = "Application",
        .size_kb = 360,
        .modified = "2026-08-14 07:21",
    },
    FileEntry{
        .name = "ntoskrnl.exe",
        .type = "Application",
        .size_kb = 11264,
        .modified = "2026-08-14 07:18",
    },
};
// "1840" -> "1,840".
std::string GroupThousands(std::uint32_t value) {
  std::string digits = std::to_string(value);
  for (std::size_t end = digits.size(); end > kDigitsPerGroup;
       end -= kDigitsPerGroup) {
    digits.insert(end - kDigitsPerGroup, ",");
  }
  return digits;
}

void TextRightAligned(const std::string& text) {
  const float width = ImGui::CalcTextSize(text.c_str()).x;
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                       ImGui::GetContentRegionAvail().x - width);
  ImGui::TextUnformatted(text.c_str());
}

template <std::size_t N>
std::vector<FileEntry> ToVector(const std::array<FileEntry, N>& entries) {
  return {entries.begin(), entries.end()};
}

}  // namespace

std::string_view FolderLabel(Folder folder) {
  switch (folder) {
    case Folder::kThisPc:
      return "This PC";
    case Folder::kDocuments:
      return "Documents";
    case Folder::kPictures:
      return "Pictures";
    case Folder::kDownloads:
      return "Downloads";
    case Folder::kSystem32:
      return "System32";
  }
  return "";
}

std::string_view FolderPath(Folder folder) {
  switch (folder) {
    case Folder::kThisPc:
      return "This PC";
    case Folder::kDocuments:
      return R"(C:\Users\csopesy\Documents)";
    case Folder::kPictures:
      return R"(C:\Users\csopesy\Pictures)";
    case Folder::kDownloads:
      return R"(C:\Users\csopesy\Downloads)";
    case Folder::kSystem32:
      return R"(C:\Windows\System32)";
  }
  return "";
}

std::vector<FileEntry> EntriesIn(Folder folder) {
  switch (folder) {
    case Folder::kThisPc:
      return ToVector(kThisPcEntries);
    case Folder::kDocuments:
      return ToVector(kDocumentsEntries);
    case Folder::kPictures:
      return ToVector(kPicturesEntries);
    case Folder::kDownloads:
      return ToVector(kDownloadsEntries);
    case Folder::kSystem32:
      return ToVector(kSystem32Entries);
  }
  return {};
}

bool IsFolder(const FileEntry& entry) { return entry.type == kFolderType; }

void SortEntries(std::vector<FileEntry>& entries, SortColumn column,
                 SortDirection direction) {
  const auto less = [column](const FileEntry& lhs, const FileEntry& rhs) {
    switch (column) {
      case SortColumn::kName:
        return lhs.name < rhs.name;
      case SortColumn::kType:
        return std::tie(lhs.type, lhs.name) < std::tie(rhs.type, rhs.name);
      case SortColumn::kSize:
        return std::tie(lhs.size_kb, lhs.name) <
               std::tie(rhs.size_kb, rhs.name);
      case SortColumn::kModified:
        return std::tie(lhs.modified, lhs.name) <
               std::tie(rhs.modified, rhs.name);
    }
    return false;
  };
  if (direction == SortDirection::kAscending) {
    std::ranges::sort(entries, less);
    return;
  }
  std::ranges::sort(entries,
                    [&less](const FileEntry& first, const FileEntry& second) {
                      return less(second, first);
                    });
}

std::string FormatSize(const FileEntry& entry) {
  if (IsFolder(entry)) {
    return "";
  }
  return GroupThousands(entry.size_kb) + " KB";
}

std::string FormatStatus(const FolderStatus& status) {
  std::string text =
      std::format("{} {}", status.items, status.items == 1 ? "item" : "items");
  if (status.selected > 0) {
    text += std::format(" | {} selected", status.selected);
  }
  return text;
}

void FolderHistory::Navigate(Folder folder) {
  if (folder == current()) {
    return;
  }
  folders_.resize(index_ + 1);
  folders_.push_back(folder);
  ++index_;
}

void FolderHistory::Back() {
  if (CanGoBack()) {
    --index_;
  }
}

void FolderHistory::Forward() {
  if (CanGoForward()) {
    ++index_;
  }
}

void FileExplorer::Draw() {
  DrawNavigationBar();

  const float panes_height = -ImGui::GetFrameHeightWithSpacing();
  ImGui::BeginChild("##tree", ImVec2(Theme::kFolderTreeWidth, panes_height),
                    ImGuiChildFlags_Borders);
  DrawFolderTree();
  ImGui::EndChild();
  ImGui::SameLine();

  const std::vector<FileEntry> entries = EntriesIn(history_.current());
  ImGui::BeginChild("##files", ImVec2(0.0F, panes_height));
  DrawFileTable(entries);
  ImGui::EndChild();

  ImGui::TextUnformatted(
      FormatStatus({.items = entries.size(), .selected = selected_.size()})
          .c_str());
}

void FileExplorer::DrawNavigationBar() {
  ImGui::BeginDisabled(!history_.CanGoBack());
  if (ImGui::ArrowButton("##back", ImGuiDir_Left)) {
    history_.Back();
    selected_.clear();
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(!history_.CanGoForward());
  if (ImGui::ArrowButton("##forward", ImGuiDir_Right)) {
    history_.Forward();
    selected_.clear();
  }
  ImGui::EndDisabled();
  ImGui::SameLine();

  std::string address(FolderPath(history_.current()));
  ImGui::SetNextItemWidth(-1.0F);
  ImGui::InputText("##address", address.data(), address.size() + 1,
                   ImGuiInputTextFlags_ReadOnly);
}

void FileExplorer::DrawFolderTree() {
  const bool this_pc_current = history_.current() == Folder::kThisPc;
  const ImGuiTreeNodeFlags root_flags =
      this_pc_current
          ? core::CombineFlags(kRootNodeFlags, ImGuiTreeNodeFlags_Selected)
          : kRootNodeFlags;
  const std::string label(FolderLabel(Folder::kThisPc));
  const bool open = ImGui::TreeNodeEx(label.c_str(), root_flags);
  if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    Navigate(Folder::kThisPc);
  }
  if (!open) {
    return;
  }
  for (const Folder folder : kChildFolders) {
    DrawFolderNode(folder);
  }
  ImGui::TreePop();
}

void FileExplorer::DrawFolderNode(Folder folder) {
  const ImGuiTreeNodeFlags flags =
      history_.current() == folder
          ? core::CombineFlags(kLeafNodeFlags, ImGuiTreeNodeFlags_Selected)
          : kLeafNodeFlags;
  const std::string label(FolderLabel(folder));
  ImGui::TreeNodeEx(label.c_str(), flags);
  if (ImGui::IsItemClicked()) {
    Navigate(folder);
  }
}

void FileExplorer::DrawFileTable(const std::vector<FileEntry>& entries) {
  if (!ImGui::BeginTable("##file_list", kColumnCount, kFileTableFlags)) {
    return;
  }
  ImGui::TableSetupScrollFreeze(0, 1);
  ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
  ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed,
                          Theme::kFileTypeColumnWidth);
  ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed,
                          Theme::kFileSizeColumnWidth);
  ImGui::TableSetupColumn("Date modified", ImGuiTableColumnFlags_WidthFixed,
                          Theme::kFileDateColumnWidth);
  ImGui::TableHeadersRow();
  ReadSortSpecs();

  std::vector<FileEntry> sorted = entries;
  SortEntries(sorted, sort_column_, sort_direction_);
  for (const FileEntry& entry : sorted) {
    DrawFileRow(entry);
  }
  ImGui::EndTable();
}

void FileExplorer::ReadSortSpecs() {
  ImGuiTableSortSpecs* specs = ImGui::TableGetSortSpecs();
  if (specs == nullptr || !specs->SpecsDirty) {
    return;
  }
  if (specs->SpecsCount > 0) {
    const ImGuiTableColumnSortSpecs& spec = *specs->Specs;
    sort_column_ = static_cast<SortColumn>(spec.ColumnIndex);
    sort_direction_ = spec.SortDirection == ImGuiSortDirection_Descending
                          ? SortDirection::kDescending
                          : SortDirection::kAscending;
  }
  specs->SpecsDirty = false;
}

void FileExplorer::DrawFileRow(const FileEntry& entry) {
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(kNameColumn);
  const std::string name(entry.name);
  const bool folder = IsFolder(entry);
  if (folder) {
    ImGui::PushStyleColor(ImGuiCol_Text, Theme::kFolderIconColor);
  }
  const bool clicked =
      ImGui::Selectable(name.c_str(), IsSelected(entry.name), kRowSelectFlags);
  if (folder) {
    ImGui::PopStyleColor();
  }
  if (clicked) {
    ToggleSelection(entry.name, ImGui::GetIO().KeyCtrl);
  }
  ImGui::TableSetColumnIndex(kTypeColumn);
  ImGui::TextUnformatted(std::string(entry.type).c_str());
  ImGui::TableSetColumnIndex(kSizeColumn);
  TextRightAligned(FormatSize(entry));
  ImGui::TableSetColumnIndex(kModifiedColumn);
  ImGui::TextUnformatted(std::string(entry.modified).c_str());
}

void FileExplorer::ToggleSelection(std::string_view name,
                                   bool add_to_selection) {
  if (!add_to_selection) {
    selected_.assign({name});
    return;
  }
  const auto found = std::ranges::find(selected_, name);
  if (found != selected_.end()) {
    selected_.erase(found);
  } else {
    selected_.push_back(name);
  }
}

void FileExplorer::Navigate(Folder folder) {
  if (folder != history_.current()) {
    selected_.clear();
  }
  history_.Navigate(folder);
}

bool FileExplorer::IsSelected(std::string_view name) const {
  return std::ranges::find(selected_, name) != selected_.end();
}

}  // namespace csopesy::apps
