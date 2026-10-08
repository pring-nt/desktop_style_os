# Build Order

Build in five phases so a working, submittable app exists after phase 3 and later phases only add polish.

Work top to bottom. A task may start only when every task in its "Depends on" column is ticked. Never start the next phase until the current one meets its exit criteria and the user approves.

## Phase 1 — Skeleton and tooling

Details: [phases/phase-1-skeleton-and-tooling.md](phases/phase-1-skeleton-and-tooling.md). _Done when `check` passes on every member's machine._

| #   | Task                                                                                                                                                                                                                                                                  | Depends on    | Status |
| --- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------- | ------ |
| 1.1 | Split the spec into `docs/spec/`, format with pinned Prettier, get user approval                                                                                                                                                                                      | —             | [x]    |
| 1.2 | Pick the newest stable LLVM major version; write `CONTRIBUTING.md` (tool setup, pinned versions, git conventions, "Verify your setup")                                                                                                                                | 1.1           | [x]    |
| 1.3 | Commit configs: `.clang-format`, `.clang-tidy`, `.prettierrc`, `.editorconfig`, `.gitignore`, `.gitattributes`                                                                                                                                                        | 1.2           | [x]    |
| 1.4 | CMake skeleton: `CMakeLists.txt`, `CMakePresets.json` (`debug`, `release`, `ci`), `cmake/CompilerWarnings.cmake`, `cmake/Dependencies.cmake` (pinned GLFW, Dear ImGui, doctest), `cmake/Tooling.cmake`; vendor glad and stb in `third_party/`; `imgui` library target | 1.1           | [ ]    |
| 1.5 | GLFW window with an ImGui demo window from `src/main.cc` (replaces template `main.cpp`)                                                                                                                                                                               | 1.4           | [ ]    |
| 1.6 | One passing doctest in `csopesy_tests`, registered with CTest                                                                                                                                                                                                         | 1.4           | [ ]    |
| 1.7 | `scripts/check.sh` and `scripts/check.ps1` running all seven gates                                                                                                                                                                                                    | 1.3, 1.5, 1.6 | [ ]    |
| 1.8 | Exit: `check` passes on every member's machine                                                                                                                                                                                                                        | 1.7           | [ ]    |

## Phase 2 — Shell core

Details: [phases/phase-2-shell-core.md](phases/phase-2-shell-core.md).

| #   | Task                                                                                       | Depends on | Status |
| --- | ------------------------------------------------------------------------------------------ | ---------- | ------ |
| 2.1 | `Clock` with injected time point + unit tests                                              | Phase 1    | [ ]    |
| 2.2 | `StateMachine` (`Bios`, `Splash`, `Desktop`, `Shutdown`, transitions, timers) + unit tests | Phase 1    | [ ]    |
| 2.3 | `Theme` and fonts (monospace for boot, default/sans for shell), applied once at startup    | Phase 1    | [ ]    |
| 2.4 | `AppWindow` base (shared window behavior)                                                  | 2.3        | [ ]    |
| 2.5 | `WindowManager` (open/close/focus/minimize, z-order, running flags) + unit tests           | 2.4        | [ ]    |
| 2.6 | `App`: owns GLFW window, ImGui context, state machine; frame loop; single cleanup path     | 2.2, 2.3   | [ ]    |
| 2.7 | Desktop state with gradient wallpaper (F03 fallback) and clock (F04)                       | 2.1, 2.6   | [ ]    |
| 2.8 | Exit: phase acceptance boxes ticked, `check` green                                         | 2.1–2.7    | [ ]    |

## Phase 3 — Required features

Details: [phases/phase-3-required-features.md](phases/phase-3-required-features.md). _Done when every row of the traceability table passes and `check` is green._

| #    | Task                                                                       | Depends on | Status |
| ---- | -------------------------------------------------------------------------- | ---------- | ------ |
| 3.1  | F03 image wallpaper (stb_image texture, cover scaling, gradient fallback)  | 2.7        | [ ]    |
| 3.2  | F06 taskbar layout                                                         | 2.5, 2.7   | [ ]    |
| 3.3  | F07 three app icon buttons and click behavior                              | 3.2        | [ ]    |
| 3.4  | F08 running-app indicators                                                 | 3.3        | [ ]    |
| 3.5  | F09 PWR in system tray + F05 shutdown flow with confirm modal              | 3.2        | [ ]    |
| 3.6  | F13 `DummyProcessTable` + unit tests                                       | Phase 2    | [ ]    |
| 3.7  | F12 + F13 Task Manager window with Processes table                         | 3.3, 3.6   | [ ]    |
| 3.8  | F10 File Explorer                                                          | 3.3        | [ ]    |
| 3.9  | F11 `TerminalCommands` + unit tests                                        | 3.6        | [ ]    |
| 3.10 | F11 Terminal window                                                        | 3.3, 3.9   | [ ]    |
| 3.11 | Exit: every traceability row (except F1, F2, D1, D2) passes, `check` green | 3.1–3.10   | [ ]    |

## Phase 4 — Boot sequence and polish

Details: [phases/phase-4-boot-and-polish.md](phases/phase-4-boot-and-polish.md).

| #   | Task                                               | Depends on | Status |
| --- | -------------------------------------------------- | ---------- | ------ |
| 4.1 | F01 BIOS POST screen                               | Phase 3    | [ ]    |
| 4.2 | F02 Splash screen                                  | 4.1        | [ ]    |
| 4.3 | F07 hover tooltips                                 | Phase 3    | [ ]    |
| 4.4 | F09 VOL/NET popups                                 | Phase 3    | [ ]    |
| 4.5 | F12 CPU and Memory color shading                   | Phase 3    | [ ]    |
| 4.6 | Optional extras (only if time allows)              | 4.1–4.5    | [ ]    |
| 4.7 | Exit: phase acceptance boxes ticked, `check` green | 4.1–4.5    | [ ]    |

## Phase 5 — Report and submission

Details: [phases/phase-5-report-and-submission.md](phases/phase-5-report-and-submission.md).

| #   | Task                                                     | Depends on | Status |
| --- | -------------------------------------------------------- | ---------- | ------ |
| 5.1 | D1 `README.txt`                                          | Phase 4    | [ ]    |
| 5.2 | D2 diagrams in `docs/report/diagrams/` (Mermaid + PNG)   | Phase 4    | [ ]    |
| 5.3 | Video walkthrough                                        | Phase 4    | [ ]    |
| 5.4 | D2 `docs/report/TECHNICAL_REPORT.md`                     | 5.2, 5.3   | [ ]    |
| 5.5 | Resize and clean-machine testing (manual test checklist) | 5.1        | [ ]    |
| 5.6 | Final `check` run                                        | 5.1–5.5    | [ ]    |

## Suggested work split (4 members)

| Area                           | Features                                                  | Depends on                                  |
| ------------------------------ | --------------------------------------------------------- | ------------------------------------------- |
| Core, build and boot           | CMake, quality gates, App loop, state machine, F1, F2, F5 | —                                           |
| Desktop and taskbar            | F3, F4, F6–F9, WindowManager                              | Core                                        |
| Task Manager and File Explorer | F12, F13, F10, DummyProcessTable                          | AppWindow base                              |
| Terminal and report            | F11, TerminalCommands, D1, D2                             | AppWindow base; all features for the report |
