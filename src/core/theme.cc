#include "core/theme.h"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <string>

#include "imgui.h"

#include "core/paths.h"

namespace csopesy::core {

namespace {

constexpr float kWindowRounding = 8.0F;
constexpr float kFrameRounding = 4.0F;
constexpr float kPopupRounding = 6.0F;
constexpr ImVec2 kWindowPadding{10.0F, 10.0F};
constexpr ImVec2 kFramePadding{8.0F, 4.0F};
constexpr ImVec2 kItemSpacing{8.0F, 6.0F};

}  // namespace

void Theme::Apply(const std::filesystem::path& shell_font_path) {
  ImGuiIO& io = ImGui::GetIO();
  if (!shell_font_path.empty()) {
    shell_font_data_ = ReadFileBytes(shell_font_path);
  }
  if (!shell_font_data_.empty() &&
      shell_font_data_.size() <= static_cast<std::size_t>(INT_MAX)) {
    ImFontConfig config;
    config.FontDataOwnedByAtlas = false;
    // ImGui only names fonts it reads itself; the name shows in its debug
    // tools.
    const std::string name = shell_font_path.filename().string();
    std::copy_n(name.begin(), std::min(name.size(), std::size(config.Name) - 1),
                std::begin(config.Name));
    shell_font_ = io.Fonts->AddFontFromMemoryTTF(
        shell_font_data_.data(), static_cast<int>(shell_font_data_.size()),
        kShellFontSize, &config);
  }
  if (shell_font_ == nullptr) {
    shell_font_ = io.Fonts->AddFontDefaultVector();
  }
  boot_font_ = io.Fonts->AddFontDefaultBitmap();
  io.FontDefault = shell_font_;

  ImGuiStyle& style = ImGui::GetStyle();
  ImGui::StyleColorsDark(&style);
  style.FontSizeBase = kShellFontSize;
  style.WindowRounding = kWindowRounding;
  style.ChildRounding = kFrameRounding;
  style.FrameRounding = kFrameRounding;
  style.GrabRounding = kFrameRounding;
  style.ScrollbarRounding = kFrameRounding;
  style.TabRounding = kFrameRounding;
  style.PopupRounding = kPopupRounding;
  style.WindowPadding = kWindowPadding;
  style.FramePadding = kFramePadding;
  style.ItemSpacing = kItemSpacing;
}

}  // namespace csopesy::core
