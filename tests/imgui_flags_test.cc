#include "core/imgui_flags.h"

#include "imgui.h"
#include <doctest/doctest.h>

namespace csopesy::core {
namespace {

TEST_CASE("CombineFlags sets exactly the given bits") {
  CHECK(CombineFlags(ImGuiWindowFlags_NoMove) == ImGuiWindowFlags_NoMove);
  CHECK(CombineFlags(ImGuiWindowFlags_NoTitleBar, ImGuiWindowFlags_NoMove) ==
        ImGuiWindowFlags_NoTitleBar + ImGuiWindowFlags_NoMove);
  CHECK(CombineFlags(ImGuiWindowFlags_NoMove, ImGuiWindowFlags_NoMove) ==
        ImGuiWindowFlags_NoMove);
}

}  // namespace
}  // namespace csopesy::core
