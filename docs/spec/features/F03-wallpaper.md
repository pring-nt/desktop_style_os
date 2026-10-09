# F03 — Wallpaper (required)

| Field            | Value                                                                                                                                                  |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------ |
| ID               | F03                                                                                                                                                    |
| Summary          | The full-window base layer, drawn first every frame: an image wallpaper with a gradient fallback.                                                      |
| Spec requirement | Desktop: full-screen base layer, rendered first each frame; fills the entire application window; wallpaper: gradient, ImGui pattern, or image texture. |
| Phase            | 2 (task 2.7, gradient) and 3 (task 3.1, image)                                                                                                         |
| Depends on       | `App` frame loop (2.6), `Theme` (2.3); stb_image and glad vendored (1.4)                                                                               |
| Files it touches | `src/shell/desktop.h`, `src/shell/desktop.cc`, `assets/wallpapers/`, `third_party/stb/`, `src/core/theme.*` (gradient colors)                          |

## Requirements

- Implemented as a borderless, non-movable ImGui window covering the whole viewport (the taskbar draws on top), or via `ImGui::GetBackgroundDrawList()`.
- Primary option: a loaded image texture (stb_image → `glTexImage2D` → `ImGui::Image`/`AddImage`), scaled to cover the viewport while keeping aspect ratio.
- Fallback if the image fails to load: a vertical color gradient via `AddRectFilledMultiColor`, so the app never shows a blank screen.
- Optional: the image is chosen with the wallpaper picker ([F15](F15-wallpaper-picker.md)).
- Optional desktop label (e.g. "CSOPESY OS v1.0 — System Online") in a corner.

## Acceptance criteria (Desktop, shared with F04, F05)

- [x] Wallpaper fills the entire window at any size and is the first layer drawn.

## Unit tests to write

- None required by the spec (UI drawing). Verified by the manual test checklist: "Missing wallpaper file falls back to the gradient without crashing." and the resize check.
