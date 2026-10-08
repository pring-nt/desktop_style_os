# F06 — Taskbar layout

| Field            | Value                                                                                                         |
| ---------------- | ------------------------------------------------------------------------------------------------------------- |
| ID               | F06                                                                                                           |
| Summary          | A fixed bottom panel, always on top, holding app launcher icons on the left and the system tray on the right. |
| Spec requirement | Taskbar: fixed panel at top or bottom.                                                                        |
| Phase            | 3 (task 3.2)                                                                                                  |
| Depends on       | `WindowManager` (2.5), Desktop state (2.7), `Theme` (2.3)                                                     |
| Files it touches | `src/shell/taskbar.h`, `src/shell/taskbar.cc`, `src/core/theme.*`                                             |

The taskbar is a fixed bottom panel, always on top, with at least three clickable icon buttons: two open unique app screens and one opens the Task Manager. It also shows which apps are running and holds the system tray.

## Requirements

- Full width, ~56–64 px tall, pinned to the bottom of the viewport; dark semi-opaque background with a thin top border.
- Left: app launcher icons. Right: system tray (VOL, NET, PWR).
- Flags: `NoTitleBar | NoResize | NoMove | NoScrollbar | NoSavedSettings`; recomputed from `GetMainViewport()` each frame.

## Acceptance criteria (Taskbar, shared with F07, F08)

- [ ] Taskbar stays visible above every app window and at the bottom after resizing.

## Unit tests to write

- None required by the spec (UI drawing). Verified by the manual resize check.
