#include "core/state_machine.h"

#include <doctest/doctest.h>

namespace csopesy::core {
namespace {

constexpr Seconds kBios{2.0F};
constexpr Seconds kSplash{1.0F};
constexpr Seconds kShutdown{0.5F};
constexpr Seconds kTick{0.25F};
constexpr Seconds kLongIdle{600.0F};

StateMachine MakeMachine() {
  return StateMachine(
      StateDurations{.bios = kBios, .splash = kSplash, .shutdown = kShutdown});
}

StateMachine MakeMachineOnDesktop() {
  StateMachine machine = MakeMachine();
  machine.Update(kBios);
  machine.Update(kSplash);
  return machine;
}

TEST_CASE("StateMachine starts on the BIOS screen") {
  CHECK(MakeMachine().state() == AppState::kBios);
}

TEST_CASE("StateMachine moves from BIOS to Splash after its duration") {
  StateMachine machine = MakeMachine();
  machine.Update(kBios - kTick);
  CHECK(machine.state() == AppState::kBios);
  machine.Update(kTick);
  CHECK(machine.state() == AppState::kSplash);
  CHECK(machine.time_in_state() == Seconds{0.0F});
}

TEST_CASE("StateMachine skips the BIOS screen on input") {
  StateMachine machine = MakeMachine();
  machine.SkipBios();
  CHECK(machine.state() == AppState::kSplash);
  machine.SkipBios();
  CHECK(machine.state() == AppState::kSplash);
}

TEST_CASE("StateMachine moves from Splash to Desktop after its duration") {
  StateMachine machine = MakeMachine();
  machine.Update(kBios);
  machine.Update(kSplash - kTick);
  CHECK(machine.state() == AppState::kSplash);
  machine.Update(kTick);
  CHECK(machine.state() == AppState::kDesktop);
}

TEST_CASE("StateMachine stays on the Desktop without PWR") {
  StateMachine machine = MakeMachineOnDesktop();
  machine.Update(kLongIdle);
  CHECK(machine.state() == AppState::kDesktop);
  CHECK_FALSE(machine.should_exit());
}

TEST_CASE("StateMachine moves to Shutdown when PWR is confirmed") {
  StateMachine machine = MakeMachineOnDesktop();
  machine.RequestShutdown();
  CHECK(machine.is_shutdown_pending());
  machine.ConfirmShutdown();
  CHECK(machine.state() == AppState::kShutdown);
  CHECK_FALSE(machine.is_shutdown_pending());
}

TEST_CASE("StateMachine stays on the Desktop when PWR is cancelled") {
  StateMachine machine = MakeMachineOnDesktop();
  machine.RequestShutdown();
  machine.CancelShutdown();
  CHECK_FALSE(machine.is_shutdown_pending());
  machine.ConfirmShutdown();
  CHECK(machine.state() == AppState::kDesktop);
}

TEST_CASE("StateMachine ignores PWR during boot") {
  StateMachine machine = MakeMachine();
  machine.RequestShutdown();
  machine.ConfirmShutdown();
  CHECK_FALSE(machine.is_shutdown_pending());
  CHECK(machine.state() == AppState::kBios);
}

TEST_CASE("StateMachine requests exit after the shutdown screen") {
  StateMachine machine = MakeMachineOnDesktop();
  machine.RequestShutdown();
  machine.ConfirmShutdown();
  machine.Update(kShutdown - kTick);
  CHECK_FALSE(machine.should_exit());
  machine.Update(kTick);
  CHECK(machine.should_exit());
}

}  // namespace
}  // namespace csopesy::core
