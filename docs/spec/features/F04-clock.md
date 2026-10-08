# F04 — Real-time clock (required)

| Field            | Value                                                                                                  |
| ---------------- | ------------------------------------------------------------------------------------------------------ |
| ID               | F04                                                                                                    |
| Summary          | Local date and time, read every frame, in a fixed top-right panel.                                     |
| Spec requirement | Desktop: real-time clock, updated every frame, fixed corner.                                           |
| Phase            | 2 (tasks 2.1, 2.7)                                                                                     |
| Depends on       | `Clock` (2.1), `Theme` (2.3), Desktop state (2.7)                                                      |
| Files it touches | `src/core/clock.h`, `src/core/clock.cc`, `src/shell/desktop.cc` (clock overlay), `tests/clock_test.cc` |

## Requirements

- Read local time every frame with `std::chrono::system_clock::now()`; format as `Thursday, Oct 08, 2026 | 07:43 PM`.
- Fixed in the top-right corner inside a small dark, semi-transparent rounded panel.
- Anchored to viewport size so it stays in the corner when the window resizes.

## Acceptance criteria (Desktop, shared with F03, F05)

- [x] Clock seconds/minutes visibly advance without user input.

## Unit tests to write

- `Clock`: fixed time point formats as `Thursday, Oct 08, 2026 | 07:43 PM`; midnight and noon edge cases.
- `Clock` takes an injected time point; tests never read the wall clock.
