#include "core/state_machine.h"

namespace csopesy::core {

StateMachine::StateMachine(StateDurations durations) : durations_(durations) {}

void StateMachine::Update(Seconds elapsed) {
  time_in_state_ += elapsed;
  switch (state_) {
    case AppState::kBios:
      if (time_in_state_ >= durations_.bios) {
        TransitionTo(AppState::kSplash);
      }
      break;
    case AppState::kSplash:
      if (time_in_state_ >= durations_.splash) {
        TransitionTo(AppState::kDesktop);
      }
      break;
    case AppState::kDesktop:
      break;
    case AppState::kShutdown:
      if (time_in_state_ >= durations_.shutdown) {
        should_exit_ = true;
      }
      break;
  }
}

void StateMachine::SkipBios() {
  if (state_ == AppState::kBios) {
    TransitionTo(AppState::kSplash);
  }
}

void StateMachine::RequestShutdown() {
  if (state_ == AppState::kDesktop) {
    is_shutdown_pending_ = true;
  }
}

void StateMachine::CancelShutdown() { is_shutdown_pending_ = false; }

void StateMachine::ConfirmShutdown() {
  if (state_ == AppState::kDesktop && is_shutdown_pending_) {
    is_shutdown_pending_ = false;
    TransitionTo(AppState::kShutdown);
  }
}

void StateMachine::TransitionTo(AppState next) {
  state_ = next;
  time_in_state_ = Seconds{0.0F};
}

}  // namespace csopesy::core
