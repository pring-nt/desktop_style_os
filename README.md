# CSOPESY Desktop OS Emulator

A single-window desktop OS mock-up in C++20 (GLFW + OpenGL 3.3 + Dear ImGui). It boots through a BIOS POST screen and a splash screen into a desktop with a wallpaper, a clock, a taskbar, and File Explorer, Terminal, Task Manager and Minesweeper windows, and shuts down through the PWR button. All system data is placeholder.

## Group and authors

**CSOPESY - Section S01 - Group 12**

| Author                     | Section |
| -------------------------- | ------- |
| Trinidad, Nathan           | S01     |
| Singh, Nathaniel           | S01     |
| Quilantang, Jann Miro      | S01     |
| Saguin, VL Kirsten Camille | S01     |

## Requirements

- **OS:** Windows 10/11 (tested), or Linux / macOS with OpenGL 3.3
- **C++20 compiler:** GCC 13+, Clang 17+ or MSVC 2022
- **CMake** 3.24 or newer
- **Ninja**
- **Internet access on the first configure:** CMake downloads GLFW, Dear ImGui and doctest at pinned versions. Later builds work offline.

## Build and run

From the repository root:

```sh
cmake --preset release
cmake --build --preset release
```

Then run the executable:

| Platform      | Command                        |
| ------------- | ------------------------------ |
| Windows       | `build\release\csopesy_os.exe` |
| Linux / macOS | `./build/release/csopesy_os`   |

The build copies `assets/` next to the executable, so it can be started from any working directory.

## Entry point

[`src/main.cc`](src/main.cc) holds `int main()`. It creates the `App` class ([`src/core/app.cc`](src/core/app.cc)), which opens the window and runs everything until shutdown.

## Customizing

- **BIOS fun facts:** [`assets/fun_facts.txt`](assets/fun_facts.txt), one fact per line (lines starting with `#` are comments). Each boot shows one at random. Rebuild after editing, or edit the copy in `build/release/assets/` to try a change without rebuilding.
- **Wallpaper:** images live in `assets/`; the path is set by `kWallpaperPath` in [`src/core/app.cc`](src/core/app.cc).

## Third-party assets

- [`assets/fonts/Roboto-Medium.ttf`](assets/fonts/Roboto-Medium.ttf): Roboto by Christian Robertson, Apache License 2.0 ([license](assets/fonts/LICENSE-Roboto.txt)).

## Development

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for developer tooling (clang-format, clang-tidy, Prettier), the debug and test presets, and the `scripts/check` quality gates.
