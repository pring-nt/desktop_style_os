#ifndef CSOPESY_SRC_CORE_IMGUI_FLAGS_H_
#define CSOPESY_SRC_CORE_IMGUI_FLAGS_H_

namespace csopesy::core {

// ORs ImGui flag enumerators (ImGuiWindowFlags_, ImGuiTableFlags_, ...) as
// unsigned bits. ImGui's flag enums are signed ints, and bitwise operators on
// signed operands are flagged by bugprone-signed-bitwise.
template <typename First, typename... Rest>
[[nodiscard]] constexpr int CombineFlags(First first, Rest... rest) {
  return static_cast<int>((static_cast<unsigned int>(first) | ... |
                           static_cast<unsigned int>(rest)));
}

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_IMGUI_FLAGS_H_
