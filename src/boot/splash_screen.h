#ifndef CSOPESY_SRC_BOOT_SPLASH_SCREEN_H_
#define CSOPESY_SRC_BOOT_SPLASH_SCREEN_H_

#include <string>

#include "imgui.h"

#include "core/state_machine.h"

namespace csopesy::boot {

// Time between "Loading." gaining a dot.
inline constexpr core::Seconds kLoadingDotInterval{0.4F};
// How long the splash takes to fade in from black.
inline constexpr core::Seconds kSplashFadeIn{0.3F};

// "Loading.", "Loading..", "Loading...", repeating every three intervals.
[[nodiscard]] std::string LoadingText(core::Seconds elapsed);

// 0 when the splash appears, rising to 1 over kSplashFadeIn.
[[nodiscard]] float SplashOpacity(core::Seconds elapsed);

// The splash screen: a centered ASCII CSOPESY logo, the emulator's name and
// group credit, and an animated loading line.
class SplashScreen {
 public:
  static void Draw(core::Seconds elapsed, ImFont* font);
};

}  // namespace csopesy::boot

#endif  // CSOPESY_SRC_BOOT_SPLASH_SCREEN_H_
