# Phase 2 — Shell core

## Goal

`StateMachine`, `WindowManager`, `AppWindow` base, `Clock`, `Theme` and fonts, each with unit tests; Desktop state with gradient wallpaper and clock.

## Feature IDs included

- [F03](../features/F03-wallpaper.md) (gradient fallback only)
- [F04](../features/F04-clock.md)
- Core modules from [01-architecture.md](../01-architecture.md): `App`, `StateMachine`, `Clock`, `WindowManager`, `AppWindow`, `Theme`

## Task order

- [x] 2.1 `Clock` with an injected time point + unit tests.
- [x] 2.2 `StateMachine` (`Bios`, `Splash`, `Desktop`, `Shutdown`; transitions and timers) + unit tests.
- [x] 2.3 `Theme` and fonts (monospace for boot screens, ImGui default or sans for the shell), applied once at startup.
- [x] 2.4 `AppWindow` base (shared window behavior in `01-architecture.md`).
- [ ] 2.5 `WindowManager` (open/close/focus/minimize, z-order, running flags) + unit tests.
- [ ] 2.6 `App`: owns the GLFW window, ImGui context and state machine; runs the frame loop; one cleanup path.
- [ ] 2.7 Desktop state with gradient wallpaper (F03) and clock (F04).

## Exit criteria

- [ ] F04 acceptance: Clock seconds/minutes visibly advance without user input.
- [ ] `Clock`, `StateMachine` and `WindowManager` unit tests from `04-quality-gates.md` pass.
- [ ] `scripts/check` green.
- [ ] User approves moving to Phase 3.
