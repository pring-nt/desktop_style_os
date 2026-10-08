# F08 — Running-app indicators

| Field            | Value                                                                                |
| ---------------- | ------------------------------------------------------------------------------------ |
| ID               | F08                                                                                  |
| Summary          | A dot or underline under each taskbar icon whose window is open.                     |
| Spec requirement | Taskbar: shows running applications.                                                 |
| Phase            | 3 (task 3.4)                                                                         |
| Depends on       | F07 (3.3), `WindowManager` (2.5)                                                     |
| Files it touches | `src/shell/taskbar.cc`, `src/shell/window_manager.*`, `tests/window_manager_test.cc` |

## Requirements

- A small dot or underline under each icon whose window is open; brighter when focused.
- Driven by `WindowManager` state, not by the button itself.

## Acceptance criteria (Taskbar, shared with F06, F07)

- [x] Running indicators match which windows are open.

## Unit tests to write

- `WindowManager`: running flags match open windows.
