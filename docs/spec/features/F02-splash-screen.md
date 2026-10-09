# F02 — Splash / loading screen

| Field            | Value                                                                                                         |
| ---------------- | ------------------------------------------------------------------------------------------------------------- |
| ID               | F02                                                                                                           |
| Summary          | An ASCII-logo splash with a loading indicator, then hands off to the desktop.                                 |
| Spec requirement | (Reference images) BIOS POST and splash screens. Not on the requirements checklist; treat as expected polish. |
| Phase            | 4 (task 4.2)                                                                                                  |
| Depends on       | F01 (4.1), `StateMachine` (2.2), `Theme` (2.3)                                                                |
| Files it touches | `src/boot/splash_screen.h`, `src/boot/splash_screen.cc`, `src/core/state_machine.*` (Splash state)            |

## Requirements

- Centered ASCII-art logo ("CSOPESY") in a blue accent color.
- Under it: emulator name and version, and the group credit ("CSOPESY - Section S01 - Group 12"); member names are in `README.md`.
- Animated green "Loading." → "Loading.." → "Loading..." cycling every ~0.4 s.
- Duration ~2–3 s, then fade or cut to the Desktop state.

## Acceptance criteria (Boot sequence, shared with F01)

- [x] Launching the app always shows BIOS → Splash → Desktop in that order.
- [x] Text animates in real time; no frame freezes during the sequence.
- [x] Both screens fill the window and stay correct when it is resized.

## Unit tests to write

- `StateMachine`: Splash → Desktop (written in task 2.2).
- Drawing is verified by the manual test checklist.
