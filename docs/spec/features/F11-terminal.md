# F11 — App B — Terminal

| Field            | Value                                                                                                                                             |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| ID               | F11                                                                                                                                               |
| Summary          | Unique UI screen #2: a console with a scrolling output, an input line, history and placeholder commands.                                          |
| Spec requirement | Taskbar: two buttons open unique UI screens with placeholder info.                                                                                |
| Phase            | 3 (tasks 3.9, 3.10)                                                                                                                               |
| Depends on       | `AppWindow` base (2.4), `Clock` (2.1, for `date`/`time`), `DummyProcessTable` (3.6, for `ps`), F07 button (3.3)                                   |
| Files it touches | `src/apps/terminal.h`, `src/apps/terminal.cc`, `src/data/terminal_commands.h`, `src/data/terminal_commands.cc`, `tests/terminal_commands_test.cc` |

## Shared window behavior (AppWindow base)

- Movable, resizable ImGui window with title bar; close (X) sets `is_open_ = false`.
- A minimize button (custom, in the title area or menu bar) hides it but keeps it "running" on the taskbar.
- Clicking a window brings it to front; it cannot be dragged below the taskbar.
- Default size and position set with `ImGuiCond_FirstUseEver`, staggered so new windows don't stack exactly.

## Requirements

- Look: black background, monospace green or light-grey text, a prompt such as `C:\CSOPESY>`, and a startup banner with the OS name and version.
- Layout: a scrolling output region (`BeginChild` with auto-scroll to bottom on new output) above a single-line `InputText` with `ImGuiInputTextFlags_EnterReturnsTrue`; focus returns to the input after each command.
- Command history: Up/Down arrows cycle previous commands via an `InputText` history callback.
- Placeholder commands, all printing fake output:

| Command         | Output                                             |
| --------------- | -------------------------------------------------- |
| `help`          | List of available commands                         |
| `ver`           | `CSOPESY OS v1.0`                                  |
| `date` / `time` | Current date / time from `Clock`                   |
| `echo <text>`   | Echoes the text                                    |
| `ps`            | The Task Manager's dummy process list in text form |
| `whoami`        | `csopesy\user`                                     |
| `cls`           | Clears the scrollback                              |
| `exit`          | Closes the Terminal window (not the OS)            |
| anything else   | `'<cmd>' is not recognized as a command.`          |

- Command parsing and output live in `data/TerminalCommands` as plain functions (input string → output lines) with no ImGui calls, so they are unit-tested directly.

## Acceptance criteria (Unique app screens, shared with F10)

- [ ] Each screen has its own layout, not a copy of the other or of the Task Manager.
- [ ] Both show placeholder data and respond to basic interaction (select, type, sort).
- [ ] Open, minimize, restore and close all work from the window and the taskbar.

## Unit tests to write

- `TerminalCommands`: each command's exact output lines; unknown command message; `cls` clears; history order.
