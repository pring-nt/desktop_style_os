# F05 — PWR shutdown (required)

| Field            | Value                                                                                                                                                                                   |
| ---------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ID               | F05                                                                                                                                                                                     |
| Summary          | The graceful exit: PWR, a confirmation modal, a brief shutdown screen, then the normal cleanup path.                                                                                    |
| Spec requirement | Desktop: PWR button as shutdown; no force exit.                                                                                                                                         |
| Phase            | 3 (task 3.5)                                                                                                                                                                            |
| Depends on       | `StateMachine` (2.2), `App` cleanup path (2.6), F06 taskbar (3.2), F09 PWR button                                                                                                       |
| Files it touches | `src/shell/taskbar.cc` (PWR button, confirm modal), `src/core/state_machine.*` (Shutdown state), `src/core/app.cc` (`glfwSetWindowShouldClose`, cleanup), `tests/state_machine_test.cc` |

## Requirements

- A "PWR" button (red text) in the taskbar's system tray area.
- Clicking opens a confirmation modal ("Shut down CSOPESY OS?" — Shut Down / Cancel).
- Confirm → `Shutdown` state: brief "Shutting down..." screen (~1 s), then `glfwSetWindowShouldClose(window, true)` and the normal cleanup path.
- PWR is the designed, graceful exit. The title-bar X and Alt+F4 are not intercepted; they end the loop through the same cleanup path, so nothing is ever force-killed.

## Acceptance criteria (Desktop, shared with F03, F04)

- [ ] PWR → Shut Down closes the app cleanly with exit code 0; Cancel returns to the desktop.

## Unit tests to write

- `StateMachine`: PWR confirm → Shutdown; Cancel stays on Desktop.
