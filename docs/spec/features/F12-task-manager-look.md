# F12 — Task Manager look and layout

| Field            | Value                                                                                               |
| ---------------- | --------------------------------------------------------------------------------------------------- |
| ID               | F12                                                                                                 |
| Summary          | A window modeled on the Windows Task Manager "Processes" view.                                      |
| Spec requirement | Task Manager: closely resembles Windows Task Manager; Taskbar: third button opens the Task Manager. |
| Phase            | 3 (task 3.7); color shading in 4 (task 4.5)                                                         |
| Depends on       | `AppWindow` base (2.4), F07 button (3.3), F13 (3.6)                                                 |
| Files it touches | `src/apps/task_manager.h`, `src/apps/task_manager.cc`, `src/core/theme.*` (shading colors)          |

The Task Manager is a window that closely resembles Windows Task Manager, with a Processes table showing each process's CPU and memory usage from dummy values.

The spec asks for exactly two things here: a Windows-like look and a placeholder Processes table with dummy CPU and memory values. Nothing else is required; anything beyond is optional polish.

## Requirements

- Modeled on the Windows Task Manager "Processes" view: title "Task Manager", a slim tab strip with "Processes" selected, then the table filling the window.
- Column headers show the totals above their names, as Windows does (e.g. `31%` over CPU, `58%` over Memory).
- Rows grouped under "Apps (n)" and "Background processes (n)" header rows.
- CPU and Memory cells shaded pale to darker yellow as the value rises.
- Default size ~720 × 480, resizable.

## Acceptance criteria (Task Manager, shared with F13)

- [ ] Opens from its taskbar button and is recognizably the Windows Task Manager Processes view.

## Unit tests to write

- None required by the spec (UI drawing). Totals are tested under F13.
