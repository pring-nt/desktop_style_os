#ifndef CSOPESY_SRC_APPS_FILE_EXPLORER_H_
#define CSOPESY_SRC_APPS_FILE_EXPLORER_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"

#include "apps/app_window.h"

namespace csopesy::apps {

// The folders in the File Explorer's tree. This PC is the root; the others
// are its children.
enum class Folder : std::uint8_t {
  kThisPc,
  kDocuments,
  kPictures,
  kDownloads,
  kSystem32,
};

inline constexpr std::string_view kFolderType = "File folder";

// One row of the file list. Every entry is placeholder data.
struct FileEntry {
  std::string_view name;
  std::string_view type;
  std::uint32_t size_kb;
  // "YYYY-MM-DD HH:MM", so text order is date order.
  std::string_view modified;
};

enum class SortColumn : std::uint8_t { kName, kType, kSize, kModified };
enum class SortDirection : std::uint8_t { kAscending, kDescending };

[[nodiscard]] std::string_view FolderLabel(Folder folder);
// The address bar text, e.g. "C:\Users\csopesy\Documents".
[[nodiscard]] std::string_view FolderPath(Folder folder);
[[nodiscard]] std::vector<FileEntry> EntriesIn(Folder folder);
[[nodiscard]] bool IsFolder(const FileEntry& entry);

// Sorts by `column`; rows that tie are ordered by name.
void SortEntries(std::vector<FileEntry>& entries, SortColumn column,
                 SortDirection direction);

// "1,840 KB" for files, empty for folders, as the Size column shows it.
[[nodiscard]] std::string FormatSize(const FileEntry& entry);
struct FolderStatus {
  std::size_t items;
  std::size_t selected;
};

// "12 items | 3 selected", or "12 items" when nothing is selected.
[[nodiscard]] std::string FormatStatus(const FolderStatus& status);

// Back/forward history of visited folders, like a browser's.
class FolderHistory {
 public:
  explicit FolderHistory(Folder start) : folders_{start} {}

  // Visits `folder` and forgets any forward entries. Visiting the current
  // folder again changes nothing.
  void Navigate(Folder folder);
  void Back();
  void Forward();

  [[nodiscard]] bool CanGoBack() const { return index_ > 0; }
  [[nodiscard]] bool CanGoForward() const {
    return index_ + 1 < folders_.size();
  }
  [[nodiscard]] Folder current() const { return folders_.at(index_); }

 private:
  std::vector<Folder> folders_;
  std::size_t index_ = 0;
};

// Unique screen #1, modeled on Windows File Explorer: back/forward buttons
// and an address bar on top, a folder tree on the left, a sortable file list
// on the right and a status bar at the bottom.
class FileExplorer : public AppWindow {
 public:
  static constexpr ImVec2 kDefaultSize{880.0F, 480.0F};

  FileExplorer() : AppWindow("File Explorer", kDefaultSize) {}

  [[nodiscard]] Folder current_folder() const { return history_.current(); }
  [[nodiscard]] std::size_t selected_count() const { return selected_.size(); }

 protected:
  void Draw() override;

 private:
  void DrawNavigationBar();
  void DrawFolderTree();
  void DrawFolderNode(Folder folder);
  void DrawFileTable(const std::vector<FileEntry>& entries);
  void ReadSortSpecs();
  void DrawFileRow(const FileEntry& entry);
  void ToggleSelection(std::string_view name, bool add_to_selection);
  void Navigate(Folder folder);
  [[nodiscard]] bool IsSelected(std::string_view name) const;

  FolderHistory history_{Folder::kDocuments};
  SortColumn sort_column_ = SortColumn::kName;
  SortDirection sort_direction_ = SortDirection::kAscending;
  std::vector<std::string_view> selected_;
};

}  // namespace csopesy::apps

#endif  // CSOPESY_SRC_APPS_FILE_EXPLORER_H_
