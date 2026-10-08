#ifndef CSOPESY_SRC_CORE_STATE_MACHINE_H_
#define CSOPESY_SRC_CORE_STATE_MACHINE_H_

#include <chrono>
#include <cstdint>

namespace csopesy::core {

enum class AppState : std::uint8_t { kBios, kSplash, kDesktop, kShutdown };

using Seconds = std::chrono::duration<float>;

inline constexpr Seconds kDefaultBiosDuration{4.0F};
inline constexpr Seconds kDefaultSplashDuration{2.5F};
inline constexpr Seconds kDefaultShutdownDuration{1.0F};

// How long each timed state lasts before it moves on.
struct StateDurations {
  Seconds bios = kDefaultBiosDuration;
  Seconds splash = kDefaultSplashDuration;
  Seconds shutdown = kDefaultShutdownDuration;
};

// The app's top-level flow: Bios -> Splash -> Desktop -> Shutdown. Boot
// states advance on timers; the desktop leaves only through a confirmed PWR
// request. Holds no UI code, so it is unit-tested without a window.
class StateMachine {
 public:
  StateMachine() = default;
  explicit StateMachine(StateDurations durations);

  // Advances timers by one frame's elapsed time.
  void Update(Seconds elapsed);

  // Any key or click during the BIOS screen jumps to the splash.
  void SkipBios();

  // PWR flow: a request opens the confirmation; cancel closes it and stays on
  // the desktop; confirm moves to Shutdown. Ignored outside their step.
  void RequestShutdown();
  void CancelShutdown();
  void ConfirmShutdown();

  [[nodiscard]] AppState state() const { return state_; }
  // Time spent in the current state, for boot and shutdown animations.
  [[nodiscard]] Seconds time_in_state() const { return time_in_state_; }
  [[nodiscard]] bool is_shutdown_pending() const {
    return is_shutdown_pending_;
  }
  // True once the shutdown screen has finished; the app then closes.
  [[nodiscard]] bool should_exit() const { return should_exit_; }

 private:
  void TransitionTo(AppState next);

  StateDurations durations_;
  AppState state_ = AppState::kBios;
  Seconds time_in_state_{0.0F};
  bool is_shutdown_pending_ = false;
  bool should_exit_ = false;
};

}  // namespace csopesy::core

#endif  // CSOPESY_SRC_CORE_STATE_MACHINE_H_
