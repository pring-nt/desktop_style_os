#ifndef CSOPESY_TESTS_IMGUI_TEST_SUPPORT_H_
#define CSOPESY_TESTS_IMGUI_TEST_SUPPORT_H_

#include "imgui.h"

namespace csopesy::testing {

// A Dear ImGui context that runs frames without a window or GPU, so code that
// calls ImGui can be tested headless. Debug builds keep IM_ASSERT on, so
// unbalanced Begin/End or Push/Pop calls fail the test.
class HeadlessImGui {
 public:
  static constexpr ImVec2 kDisplaySize{1280.0F, 720.0F};
  static constexpr float kFrameTime = 1.0F / 60.0F;

  HeadlessImGui() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = kDisplaySize;
    io.DeltaTime = kFrameTime;
    // Stands in for a renderer backend: ImGui then queues font textures for
    // upload instead of requiring a prebuilt atlas.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
  }
  ~HeadlessImGui() { ImGui::DestroyContext(); }
  HeadlessImGui(const HeadlessImGui&) = delete;
  HeadlessImGui& operator=(const HeadlessImGui&) = delete;
  HeadlessImGui(HeadlessImGui&&) = delete;
  HeadlessImGui& operator=(HeadlessImGui&&) = delete;

  static void BeginFrame() { ImGui::NewFrame(); }
  static void EndFrame() { ImGui::Render(); }
};

}  // namespace csopesy::testing

#endif  // CSOPESY_TESTS_IMGUI_TEST_SUPPORT_H_
