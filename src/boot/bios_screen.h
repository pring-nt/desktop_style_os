#ifndef CSOPESY_SRC_BOOT_BIOS_SCREEN_H_
#define CSOPESY_SRC_BOOT_BIOS_SCREEN_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "imgui.h"

#include "core/state_machine.h"

namespace csopesy::boot {

inline constexpr std::uint32_t kTotalMemoryKb = 64000;
// Time between two POST lines appearing.
inline constexpr core::Seconds kBiosLineInterval{0.12F};
// How long the memory test takes to count up to kTotalMemoryKb.
inline constexpr core::Seconds kMemoryTestDuration{1.5F};

// The memory counted so far, `elapsed` after the memory test started.
[[nodiscard]] std::uint32_t MemoryTestKb(core::Seconds elapsed);

// The POST lines shown `elapsed` after the BIOS screen appeared. Lines
// appear one by one; the memory test line counts up before the rest
// continue.
[[nodiscard]] std::vector<std::string> VisibleBiosLines(core::Seconds elapsed);

// Whether the blinking cursor after the last line is shown.
[[nodiscard]] bool BiosCursorVisible(core::Seconds elapsed);

// "Fun Fact: <fact>", the footer line at the bottom of the BIOS screen.
[[nodiscard]] std::string FunFactFooter(std::string_view fact);

// The retro BIOS POST screen: light-grey pixel text on black, top-left, with
// a fun fact along the bottom.
class BiosScreen {
 public:
  static void Draw(core::Seconds elapsed, ImFont* font,
                   std::string_view fun_fact);
};

}  // namespace csopesy::boot

#endif  // CSOPESY_SRC_BOOT_BIOS_SCREEN_H_
