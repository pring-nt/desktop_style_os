# F09 — System tray

| Field            | Value                                                                           |
| ---------------- | ------------------------------------------------------------------------------- |
| ID               | F09                                                                             |
| Summary          | The right side of the taskbar: VOL and NET placeholders and the red PWR button. |
| Spec requirement | Desktop: PWR button as shutdown; no force exit (with F05).                      |
| Phase            | 3 (task 3.5, PWR); VOL/NET popups in 4 (task 4.4)                               |
| Depends on       | F06 (3.2); PWR triggers F05                                                     |
| Files it touches | `src/shell/taskbar.cc`                                                          |

## Requirements

- VOL and NET: placeholder buttons that open small popups (volume slider, "Connected: CSOPESY-LAN").
- PWR: red, triggers the shutdown flow (F5).

## Acceptance criteria

- No separate criteria in the spec; PWR is covered by F05: "PWR → Shut Down closes the app cleanly with exit code 0; Cancel returns to the desktop."

## Unit tests to write

- None required by the spec (UI drawing). The shutdown transition is tested under F05.
