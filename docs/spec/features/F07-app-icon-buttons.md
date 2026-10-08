# F07 — App icon buttons (required: ≥ 3)

| Field            | Value                                                                                                     |
| ---------------- | --------------------------------------------------------------------------------------------------------- |
| ID               | F07                                                                                                       |
| Summary          | Three taskbar buttons: File Explorer, Terminal and Task Manager, with open / minimize / restore toggling. |
| Spec requirement | Taskbar: ≥ 3 clickable icon buttons; third button opens the Task Manager.                                 |
| Phase            | 3 (task 3.3); hover tooltips in 4 (task 4.3)                                                              |
| Depends on       | F06 (3.2), `WindowManager` (2.5)                                                                          |
| Files it touches | `src/shell/taskbar.cc`, `src/shell/window_manager.*`, `assets/icons/`, `tests/window_manager_test.cc`     |

## Requirements

| Button                | Opens               | Notes               |
| --------------------- | ------------------- | ------------------- |
| App A (File Explorer) | Unique UI screen #1 | Folder icon         |
| App B (Terminal)      | Unique UI screen #2 | Own icon and color  |
| Task Manager          | Task Manager window | Activity/graph icon |

- Round or rounded-square buttons drawn with `ImageButton` (icon textures) or `Button` + draw-list shapes; colored text labels are acceptable, as in the reference.
- Hover: lighter background + tooltip with the app name.
- Click behavior: closed → open and focus; open and focused → minimize; minimized or behind → restore and focus.

## Acceptance criteria (Taskbar, shared with F06, F08)

- [x] At least 3 icon buttons; each opens its window on click.

## Unit tests to write

- `WindowManager`: taskbar click toggles open → focus → minimize → restore.
