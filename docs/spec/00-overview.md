# 00 — Overview

## Overview

We are building a single-window C++ application that looks and behaves like a minimal desktop OS shell: it boots through a retro BIOS screen and splash, then lands on a desktop with a wallpaper, live clock, taskbar, app windows and a Task Manager, and exits only through its PWR button. It is a convincing real-time graphical mockup, not a functional OS.

**Goals**

- Every item in the spec's "Checklist of Requirements" works and is demonstrable (Desktop, Taskbar, Task Manager).
- The app acts as its own compositor: one frame loop that draws the desktop first, then windows, then the taskbar, every frame.
- Clean, modular code that passes deterministic format, lint and test checks, so each member can own a feature without merge conflicts.

**In scope:** boot sequence, desktop, taskbar with at least 3 icon buttons, 2 unique app screens, Task Manager with dummy data, graceful PWR shutdown, the SOURCE submission and a Markdown Technical Report.

**Out of scope:** real process scheduling, a real file system, networking, real CPU/memory readings. All system data is placeholder.

**Source:** CSOPESY Semi-Major Output 2 spec (updated Sept 18, 2026), pages 1–3.

## Tech stack and build setup

The stack is fixed by the spec: C++ on GLFW + OpenGL + Dear ImGui; everything else below is a recommended default.

| Layer             | Choice                                                                        | Role                                                                                      |
| ----------------- | ----------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------- |
| Language          | C++20                                                                         | `std::chrono`, `std::format` where the compiler supports it, `std::filesystem` for assets |
| Windowing / input | GLFW 3.4                                                                      | Creates the app window and GL context                                                     |
| Rendering         | OpenGL 3.3 core                                                               | Clears the frame, holds the wallpaper texture                                             |
| UI                | Dear ImGui (pinned master release tag; docking branch not used)               | All desktop, taskbar and window drawing                                                   |
| Image loading     | stb_image                                                                     | Loads the wallpaper into a GL texture                                                     |
| GL loader         | glad (generated for GL 3.3 core)                                              | Loads OpenGL function pointers                                                            |
| Build             | CMake ≥ 3.24 + Ninja, `CMakePresets.json`                                     | One reproducible build on every machine                                                   |
| Style guide       | [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)   | Naming, headers, language-feature rules                                                   |
| Formatting        | clang-format, `BasedOnStyle: Google` (LLVM version pinned in CONTRIBUTING.md) | Enforced style, checked in CI-style script                                                |
| Linting           | clang-tidy (same version), including `google-*` checks                        | Static analysis, warnings as errors                                                       |
| Tests             | doctest                                                                       | Unit tests for non-UI logic                                                               |

**Repository layout**

```
csopesy-os/
  CMakeLists.txt
  CMakePresets.json
  .clang-format
  .clang-tidy
  .prettierrc
  .editorconfig
  .gitignore        build/, IDE folders, binaries, CLAUDE.md
  .gitattributes    LF line endings for all text files, binary assets
  README.md        names, build/run steps, entry point
  CONTRIBUTING.md   tool setup (LLVM, CMake, Ninja, bun), pinned versions, git conventions
  CLAUDE.md         agent instructions (local only, gitignored)
  cmake/            CompilerWarnings.cmake, Dependencies.cmake, Tooling.cmake
  third_party/      glad/, stb/ (vendored, unmodified)
  assets/           wallpaper.jpg, icons/, fonts/
  docs/
    spec/           split spec files (see Agent setup directives)
    report/         TECHNICAL_REPORT.md + diagrams/
  scripts/          check.sh, check.ps1
  src/
    main.cc         entry point: creates App and runs it
    core/           App, StateMachine, Clock, Theme
    boot/           BiosScreen, SplashScreen
    shell/          Desktop, Taskbar, WindowManager
    apps/           AppWindow, TaskManager, FileExplorer, Terminal
    data/           DummyProcessTable, TerminalCommands
  tests/            doctest unit tests for core/ and data/
```

- Use a monospace font (e.g. a bundled pixel or console font) for the boot screens and an ImGui default or sans font for the shell.
- Assets load relative to the executable path so the build runs from any working directory.
