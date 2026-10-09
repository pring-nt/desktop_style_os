# Phase 4 — Boot sequence and polish

## Goal

BIOS POST and splash screens, hover tooltips, VOL/NET popups, Task Manager color shading, optional extras.

## Feature IDs included

- [F01](../features/F01-bios-screen.md), [F02](../features/F02-splash-screen.md)
- [F07](../features/F07-app-icon-buttons.md) (hover tooltips)
- [F09](../features/F09-system-tray.md) (VOL/NET popups)
- [F12](../features/F12-task-manager-look.md) (CPU and Memory shading)

## Task order

- [x] 4.1 F01 BIOS POST screen.
- [x] 4.2 F02 Splash screen.
- [ ] 4.3 F07 hover tooltips.
- [ ] 4.4 F09 VOL/NET popups.
- [ ] 4.5 F12 CPU and Memory color shading.
- [ ] 4.6 Optional extras, only if time allows: F01 "Fun Fact" footer, F03 desktop label, F13 jitter / column sorting / "End task" button, a Roboto sans font for the shell (with its license file).

## Exit criteria

- [x] Launching the app always shows BIOS → Splash → Desktop in that order.
- [x] Text animates in real time; no frame freezes during the sequence.
- [x] Both screens fill the window and stay correct when it is resized.
- [ ] `scripts/check` green.
- [ ] User approves moving to Phase 5.
