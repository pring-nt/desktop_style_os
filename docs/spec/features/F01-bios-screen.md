# F01 — BIOS / POST screen

| Field            | Value                                                                                                         |
| ---------------- | ------------------------------------------------------------------------------------------------------------- |
| ID               | F01                                                                                                           |
| Summary          | The app opens on a retro BIOS POST screen before the splash.                                                  |
| Spec requirement | (Reference images) BIOS POST and splash screens. Not on the requirements checklist; treat as expected polish. |
| Phase            | 4 (task 4.1)                                                                                                  |
| Depends on       | `StateMachine` (2.2), `Theme` and monospace font (2.3), `App` frame loop (2.6)                                |
| Files it touches | `src/boot/bios_screen.h`, `src/boot/bios_screen.cc`, `src/core/state_machine.*` (Bios state), `assets/fonts/` |

## Requirements

- Black full-window background, monospace light-grey text, top-left aligned.
- Content mirrors the reference: product name, release date, copyright line, a memory test that counts up (e.g. `Checking RAM : 0K` → `64000K OK`), CPU type, BIOS version, processor lines, and the IDE drive list (Primary/Secondary Master/Slave).
- Lines appear progressively (typewriter or line-by-line on a timer) for a real-time feel.
- Optional "Fun Fact" footer line, picked from `assets/fun_facts.txt` (one fact per line) so the team can edit the facts without rebuilding.
- Duration ~3–5 s, or skip on any key/click.

## Acceptance criteria (Boot sequence, shared with F02)

- [x] Launching the app always shows BIOS → Splash → Desktop in that order.
- [x] Text animates in real time; no frame freezes during the sequence.
- [x] Both screens fill the window and stay correct when it is resized.

## Unit tests to write

- `StateMachine`: BIOS → Splash after its duration (written in task 2.2; extend if skip-on-input is added to the state machine).
- Drawing is verified by the manual test checklist: "Cold launch shows BIOS → Splash → Desktop with no console errors or ImGui asserts in a debug build."
