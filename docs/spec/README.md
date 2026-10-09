# CSOPESY Desktop OS Emulator — Spec Index

Split from `docs/PROJECT_SPEC.md` (last updated Oct 8, 2026). These files are the source of truth: if the code needs to differ, propose the spec edit first.

## How the files relate

- `BUILD_ORDER.md` is the single ordered task list. Work top to bottom; each task names its dependencies and has a `[ ]` status box.
- `00`–`04` are cross-cutting: they apply to every feature.
- `features/` holds one file per feature (`F01`–`F14`) and deliverable (`D1`, `D2`). Each lists the spec requirement it satisfies, dependencies, files touched, acceptance criteria and unit tests.
- `phases/` groups features and tasks into the five milestones. A phase is done when all its acceptance boxes are ticked and `scripts/check` is green.

## Index

| File                                                                               | Contents                                                 |
| ---------------------------------------------------------------------------------- | -------------------------------------------------------- |
| [BUILD_ORDER.md](BUILD_ORDER.md)                                                   | Phases and tasks in order, with dependencies and status  |
| [00-overview.md](00-overview.md)                                                   | Goals, scope, tech stack, repository layout              |
| [01-architecture.md](01-architecture.md)                                           | State machine, frame loop, modules, layer order          |
| [02-build-system.md](02-build-system.md)                                           | CMake rules, targets, presets                            |
| [03-coding-standards.md](03-coding-standards.md)                                   | C++ and Dear ImGui rules                                 |
| [04-quality-gates.md](04-quality-gates.md)                                         | Check script, gates, configs, unit test list, tool setup |
| [features/F01-bios-screen.md](features/F01-bios-screen.md)                         | BIOS / POST screen                                       |
| [features/F02-splash-screen.md](features/F02-splash-screen.md)                     | Splash / loading screen                                  |
| [features/F03-wallpaper.md](features/F03-wallpaper.md)                             | Wallpaper                                                |
| [features/F04-clock.md](features/F04-clock.md)                                     | Real-time clock                                          |
| [features/F05-pwr-shutdown.md](features/F05-pwr-shutdown.md)                       | PWR shutdown                                             |
| [features/F06-taskbar-layout.md](features/F06-taskbar-layout.md)                   | Taskbar layout                                           |
| [features/F07-app-icon-buttons.md](features/F07-app-icon-buttons.md)               | App icon buttons                                         |
| [features/F08-running-indicators.md](features/F08-running-indicators.md)           | Running-app indicators                                   |
| [features/F09-system-tray.md](features/F09-system-tray.md)                         | System tray                                              |
| [features/F10-file-explorer.md](features/F10-file-explorer.md)                     | App A — File Explorer                                    |
| [features/F11-terminal.md](features/F11-terminal.md)                               | App B — Terminal                                         |
| [features/F12-task-manager-look.md](features/F12-task-manager-look.md)             | Task Manager look and layout                             |
| [features/F13-processes-table.md](features/F13-processes-table.md)                 | Processes table                                          |
| [features/F14-minesweeper.md](features/F14-minesweeper.md)                         | Minesweeper (optional extra)                             |
| [features/D1-source.md](features/D1-source.md)                                     | SOURCE submission                                        |
| [features/D2-technical-report.md](features/D2-technical-report.md)                 | Technical Report                                         |
| [phases/phase-1-skeleton-and-tooling.md](phases/phase-1-skeleton-and-tooling.md)   | Phase 1                                                  |
| [phases/phase-2-shell-core.md](phases/phase-2-shell-core.md)                       | Phase 2                                                  |
| [phases/phase-3-required-features.md](phases/phase-3-required-features.md)         | Phase 3                                                  |
| [phases/phase-4-boot-and-polish.md](phases/phase-4-boot-and-polish.md)             | Phase 4                                                  |
| [phases/phase-5-report-and-submission.md](phases/phase-5-report-and-submission.md) | Phase 5                                                  |

## Requirements traceability

Every bullet in the spec's checklist maps to one feature above; the boot screens are extra polish taken from the reference images.

| Spec requirement       | Spec bullet                                                                         | Feature                        |
| ---------------------- | ----------------------------------------------------------------------------------- | ------------------------------ |
| Desktop                | Full-screen base layer, rendered first each frame                                   | F3, Architecture frame loop    |
| Desktop                | Fills the entire application window                                                 | F3                             |
| Desktop                | Wallpaper: gradient, ImGui pattern, or image texture                                | F3 (image + gradient fallback) |
| Desktop                | Real-time clock, updated every frame, fixed corner                                  | F4                             |
| Desktop                | PWR button as shutdown; no force exit                                               | F5, F9                         |
| Taskbar                | Fixed panel at top or bottom                                                        | F6                             |
| Taskbar                | Shows running applications                                                          | F8                             |
| Taskbar                | ≥ 3 clickable icon buttons                                                          | F7                             |
| Taskbar                | Two buttons open unique UI screens with placeholder info                            | F10, F11                       |
| Taskbar                | Third button opens the Task Manager                                                 | F7, F12                        |
| Task Manager           | Closely resembles Windows Task Manager                                              | F12                            |
| Task Manager           | Placeholder Processes table with CPU and memory, dummy values                       | F13                            |
| SOURCE                 | Source code, README.md with names, run instructions and entry file (or GitHub link) | D1                             |
| PPT (Technical Report) | Cover, video walkthrough, architectural diagram, code snippets, design discussion   | D2                             |
| (Reference images)     | BIOS POST and splash screens                                                        | F1, F2                         |
| (Team request)         | Minesweeper game                                                                    | F14                            |

## Standing rules while building

- Follow `BUILD_ORDER.md` strictly; never start the next phase until the current one meets its exit criteria and the user approves.
- Run `scripts/check` before calling any task done. Never disable a lint check, loosen a warning or skip a test to get green; raise it with the user instead.
- Follow the Google C++ Style Guide and the project Coding standards; comments only to explain why or to document. When unsure about a style point, check the Google guide rather than guessing.
- The spec files are the source of truth: if the code needs to differ, propose the spec edit first.
- Tick `[ ]` boxes in `BUILD_ORDER.md` and the phase file as tasks finish, and add a short line to `docs/report/notes.md` on any design decision, so the Technical Report writes itself later.
- It may check PATH for `cmake`, `ninja`, a compiler, `clang-format`, `clang-tidy` and `bun`, and report anything missing or at the wrong version.
- Git: standard Conventional Commits and feature branches (`feat/taskbar`, `fix/clock-format`). Never commit, create branches, push or open PRs until the user has reviewed and approved.
