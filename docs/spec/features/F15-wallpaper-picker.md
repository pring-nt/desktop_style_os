# F15 — Wallpaper picker (optional extra)

| Field            | Value                                                                                                                                                                                                       |
| ---------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ID               | F15                                                                                                                                                                                                         |
| Summary          | Right-click the desktop to pick the wallpaper from the images in `assets/wallpapers/`; the choice is remembered between launches.                                                                           |
| Spec requirement | None (team request, Phase 4 polish).                                                                                                                                                                        |
| Phase            | 4 (task 4.9)                                                                                                                                                                                                |
| Depends on       | F03 wallpaper, `AppWindow` and `WindowManager` (Phase 2)                                                                                                                                                    |
| Files it touches | `src/data/settings.*`, `src/data/wallpaper_catalog.*`, `src/shell/desktop.*`, `src/shell/wallpaper_picker.*`, `src/core/app.*`, `assets/wallpapers/`, `tests/settings_test.cc`, `tests/wallpaper_*_test.cc` |

## Requirements

- Wallpapers are the `.jpg`, `.jpeg` and `.png` files in `assets/wallpapers/`, found when the app starts and each time the picker opens, so adding one needs no code change.
- Right-clicking the desktop background (not a window or the taskbar) opens a context menu with "Change wallpaper...".
- That opens a "Wallpaper" window with a thumbnail of each image plus a "Gradient" option; the current choice is highlighted, and clicking one changes the wallpaper at once.
- The choice is saved to `settings.ini` next to the executable and restored on the next launch. A missing or unreadable settings file, or a saved image that no longer exists, falls back to the default wallpaper; a missing default falls back to the gradient.
- Settings parsing, formatting and the wallpaper list are pure logic in `data/` with unit tests.

## Acceptance criteria

- [x] Right-clicking the desktop background opens the menu; right-clicking a window or the taskbar does not.
- [x] Picking a thumbnail or "Gradient" changes the wallpaper immediately.
- [x] The choice survives a restart, and a deleted wallpaper falls back without crashing.

## Unit tests to write

- Settings round-trip through text; unknown keys, blank lines and comments are ignored.
- The wallpaper list keeps only image files, sorted by name, and is empty for a missing folder.
