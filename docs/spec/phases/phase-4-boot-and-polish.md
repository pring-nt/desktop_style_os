# Phase 4 — Boot sequence and polish

## Goal

BIOS POST and splash screens, hover tooltips, VOL/NET popups, Task Manager color shading, optional extras, and a Minesweeper game.

## Feature IDs included

- [F01](../features/F01-bios-screen.md), [F02](../features/F02-splash-screen.md)
- [F07](../features/F07-app-icon-buttons.md) (hover tooltips)
- [F09](../features/F09-system-tray.md) (VOL/NET popups)
- [F12](../features/F12-task-manager-look.md) (CPU and Memory shading)
- [F14](../features/F14-minesweeper.md) (Minesweeper, team request)

## Task order

- [x] 4.1 F01 BIOS POST screen.
- [x] 4.2 F02 Splash screen.
- [x] 4.3 F07 hover tooltips.
- [x] 4.4 F09 VOL/NET popups.
- [x] 4.5 F12 CPU and Memory color shading.
- [ ] 4.6 Optional extras, only if time allows: F01 "Fun Fact" footer, F03 desktop label, F13 jitter / column sorting / "End task" button, a Roboto sans font for the shell (with its license file).
- [ ] 4.8 F14 Minesweeper (team request).

## Exit criteria

- [x] Launching the app always shows BIOS → Splash → Desktop in that order.
- [x] Text animates in real time; no frame freezes during the sequence.
- [x] Both screens fill the window and stay correct when it is resized.
- [x] `scripts/check` green.
- [ ] User approves moving to Phase 5.
