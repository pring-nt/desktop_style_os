# F10 — App A — File Explorer

| Field            | Value                                                                   |
| ---------------- | ----------------------------------------------------------------------- |
| ID               | F10                                                                     |
| Summary          | Unique UI screen #1: a folder tree and a sortable table of dummy files. |
| Spec requirement | Taskbar: two buttons open unique UI screens with placeholder info.      |
| Phase            | 3 (task 3.8)                                                            |
| Depends on       | `AppWindow` base (2.4), F07 button (3.3)                                |
| Files it touches | `src/apps/file_explorer.h`, `src/apps/file_explorer.cc`                 |

Two taskbar buttons must each open a unique UI screen with placeholder information; any design is acceptable, so we pick two that look clearly different and show off ImGui widgets.

## Shared window behavior (AppWindow base)

- Movable, resizable ImGui window with title bar; close (X) sets `is_open_ = false`.
- A minimize button (custom, in the title area or menu bar) hides it but keeps it "running" on the taskbar.
- Clicking a window brings it to front; it cannot be dragged below the taskbar.
- Default size and position set with `ImGuiCond_FirstUseEver`, staggered so new windows don't stack exactly.

## Requirements

- Left pane: folder tree (`TreeNode`) — This PC, Documents, Pictures, Downloads, System32.
- Right pane: table of dummy files (Name, Type, Size, Date modified) for the selected folder, with sortable headers (`ImGui::BeginTable` + `ImGuiTableFlags_Sortable`).
- Top: back/forward buttons and an address bar showing the current path (read-only text).
- Bottom status bar: "12 items | 3 selected".

## Acceptance criteria (Unique app screens, shared with F11)

- [x] Each screen has its own layout, not a copy of the other or of the Task Manager.
- [x] Both show placeholder data and respond to basic interaction (select, type, sort).
- [x] Open, minimize, restore and close all work from the window and the taskbar.

## Unit tests to write

- None required by the spec (UI drawing). Window toggling is tested through `WindowManager` (F07).
