# Phase 3 — Required features

## Goal

Image wallpaper, taskbar with 3 buttons and running indicators, PWR shutdown with confirm, Task Manager Processes table with dummy data, File Explorer and Terminal with placeholder content.

## Feature IDs included

- [F03](../features/F03-wallpaper.md) (image texture)
- [F05](../features/F05-pwr-shutdown.md), [F06](../features/F06-taskbar-layout.md), [F07](../features/F07-app-icon-buttons.md) (without tooltips), [F08](../features/F08-running-indicators.md), [F09](../features/F09-system-tray.md) (PWR only)
- [F10](../features/F10-file-explorer.md), [F11](../features/F11-terminal.md)
- [F12](../features/F12-task-manager-look.md) (without color shading), [F13](../features/F13-processes-table.md)

## Task order

- [x] 3.1 F03 image wallpaper (stb_image texture, cover scaling, gradient fallback).
- [x] 3.2 F06 taskbar layout.
- [x] 3.3 F07 three app icon buttons and click behavior.
- [x] 3.4 F08 running-app indicators.
- [x] 3.5 F09 PWR in the system tray + F05 shutdown flow with confirm modal.
- [x] 3.6 F13 `DummyProcessTable` + unit tests.
- [x] 3.7 F12 + F13 Task Manager window with Processes table.
- [ ] 3.8 F10 File Explorer.
- [ ] 3.9 F11 `TerminalCommands` + unit tests.
- [ ] 3.10 F11 Terminal window.

## Exit criteria

_Done when every row of the traceability table passes and `check` is green._

- [x] Wallpaper fills the entire window at any size and is the first layer drawn.
- [ ] Clock seconds/minutes visibly advance without user input.
- [x] PWR → Shut Down closes the app cleanly with exit code 0; Cancel returns to the desktop.
- [x] At least 3 icon buttons; each opens its window on click.
- [x] Taskbar stays visible above every app window and at the bottom after resizing.
- [x] Running indicators match which windows are open.
- [ ] Each screen has its own layout, not a copy of the other or of the Task Manager.
- [ ] Both show placeholder data and respond to basic interaction (select, type, sort).
- [ ] Open, minimize, restore and close all work from the window and the taskbar.
- [x] Opens from its taskbar button and is recognizably the Windows Task Manager Processes view.
- [x] Table shows each process with its CPU and memory usage, all dummy values.
- [x] Header totals match the sum of the rows.
- [ ] `scripts/check` green.
- [ ] User approves moving to Phase 4.
