#include "apps/file_explorer.h"

#include <cstddef>
#include <cstdint>
// doctest prints std::string_view operands with operator<<.
#include <ostream>  // IWYU pragma: keep
#include <vector>

#include "imgui.h"
#include "imgui_test_support.h"
#include <doctest/doctest.h>

#include "apps/app_window.h"

namespace csopesy::apps {
namespace {

using testing::HeadlessImGui;

constexpr std::size_t kDocumentCount = 12;
constexpr std::size_t kSelected = 3;
constexpr std::uint32_t kSmallSizeKb = 4;
constexpr std::uint32_t kMediumSizeKb = 1840;
constexpr std::uint32_t kLargeSizeKb = 1234567;
constexpr int kFrames = 3;
constexpr WorkArea kArea{
    .min = ImVec2(0.0F, 0.0F),
    .max = ImVec2(1280.0F, 664.0F),
};

FileEntry FileOfSize(std::uint32_t size_kb) {
  return {
      .name = "file.bin",
      .type = "File",
      .size_kb = size_kb,
      .modified = "2026-01-01 00:00",
  };
}

TEST_CASE("File Explorer has placeholder entries in every folder") {
  CHECK(EntriesIn(Folder::kDocuments).size() == kDocumentCount);
  CHECK_FALSE(EntriesIn(Folder::kThisPc).empty());
  CHECK_FALSE(EntriesIn(Folder::kPictures).empty());
  CHECK_FALSE(EntriesIn(Folder::kDownloads).empty());
  CHECK_FALSE(EntriesIn(Folder::kSystem32).empty());
}

TEST_CASE("File Explorer shows Windows-style folder paths") {
  CHECK(FolderPath(Folder::kDocuments) == R"(C:\Users\csopesy\Documents)");
  CHECK(FolderPath(Folder::kSystem32) == R"(C:\Windows\System32)");
  CHECK(FolderLabel(Folder::kThisPc) == "This PC");
}

TEST_CASE("SortEntries orders by name both ways") {
  std::vector<FileEntry> entries = EntriesIn(Folder::kDocuments);
  SortEntries(entries, SortColumn::kName, SortDirection::kAscending);
  CHECK(entries.front().name == "budget.xlsx");
  CHECK(entries.back().name == "todo.txt");

  SortEntries(entries, SortColumn::kName, SortDirection::kDescending);
  CHECK(entries.front().name == "todo.txt");
}

TEST_CASE("SortEntries orders by size and by date modified") {
  std::vector<FileEntry> entries = EntriesIn(Folder::kDocuments);
  SortEntries(entries, SortColumn::kSize, SortDirection::kDescending);
  CHECK(entries.front().name == "slides.pptx");
  CHECK(entries.back().name == "todo.txt");

  SortEntries(entries, SortColumn::kModified, SortDirection::kAscending);
  CHECK(entries.front().name == "letter.docx");
  CHECK(entries.back().name == "todo.txt");
}

TEST_CASE("FormatSize groups thousands and leaves folders blank") {
  CHECK(FormatSize(FileOfSize(kSmallSizeKb)) == "4 KB");
  CHECK(FormatSize(FileOfSize(kMediumSizeKb)) == "1,840 KB");
  CHECK(FormatSize(FileOfSize(kLargeSizeKb)) == "1,234,567 KB");
  CHECK(FormatSize(EntriesIn(Folder::kThisPc).front()).empty());
}

TEST_CASE("FormatStatus counts items and the selection") {
  CHECK(FormatStatus({.items = kDocumentCount, .selected = kSelected}) ==
        "12 items | 3 selected");
  CHECK(FormatStatus({.items = kDocumentCount, .selected = 0}) == "12 items");
  CHECK(FormatStatus({.items = 1, .selected = 0}) == "1 item");
}

TEST_CASE("FolderHistory goes back and forward") {
  FolderHistory history(Folder::kDocuments);
  CHECK_FALSE(history.CanGoBack());
  history.Navigate(Folder::kPictures);
  history.Back();
  CHECK(history.current() == Folder::kDocuments);
  CHECK(history.CanGoForward());
  history.Forward();
  CHECK(history.current() == Folder::kPictures);
}

TEST_CASE("FolderHistory forgets forward entries on a new visit") {
  FolderHistory history(Folder::kDocuments);
  history.Navigate(Folder::kPictures);
  history.Back();
  history.Navigate(Folder::kDownloads);
  CHECK_FALSE(history.CanGoForward());
  history.Navigate(Folder::kDownloads);
  history.Back();
  CHECK(history.current() == Folder::kDocuments);
}

TEST_CASE("File Explorer draws and starts in Documents") {
  const HeadlessImGui imgui;
  FileExplorer explorer;
  explorer.Open();
  for (int frame = 0; frame < kFrames; ++frame) {
    HeadlessImGui::BeginFrame();
    explorer.Render(kArea);
    HeadlessImGui::EndFrame();
  }
  CHECK(explorer.current_folder() == Folder::kDocuments);
  CHECK(explorer.selected_count() == 0);
}

}  // namespace
}  // namespace csopesy::apps
