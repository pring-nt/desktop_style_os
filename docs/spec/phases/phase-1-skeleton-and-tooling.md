# Phase 1 — Skeleton and tooling

## Goal

Spec split into `docs/spec/` (see Agent setup directives), CMake presets and pinned dependencies, `.clang-format` / `.clang-tidy` / `.prettierrc`, `scripts/check.*`, CONTRIBUTING.md with tool setup, one passing doctest, GLFW window with an ImGui demo window.

## Feature IDs included

None. This phase builds the infrastructure every feature uses: [02-build-system.md](../02-build-system.md) and [04-quality-gates.md](../04-quality-gates.md).

## Task order

- [x] 1.1 Split the spec into `docs/spec/`, format with pinned Prettier, get user approval.
- [x] 1.2 Pick the newest stable LLVM major version; write `CONTRIBUTING.md` (tool setup table, pinned versions, "Verify your setup", git conventions, how to open a PR).
- [x] 1.3 Commit configs: `.clang-format`, `.clang-tidy`, `.prettierrc`, `.editorconfig`, `.gitignore`, `.gitattributes` (LF line endings) (per the config baselines in `04-quality-gates.md`).
- [x] 1.4 CMake skeleton: `CMakeLists.txt`, `CMakePresets.json` (`debug`, `release`, `ci`), `cmake/CompilerWarnings.cmake`, `cmake/Dependencies.cmake` (GLFW, Dear ImGui, doctest pinned to exact tags), `cmake/Tooling.cmake` (`format`, `format-check`, `tidy`); vendor glad (GL 3.3 core) and stb in `third_party/`; `imgui` static library target.
- [x] 1.5 GLFW window with an ImGui demo window from `src/main.cc` (replaces the CLion template `main.cpp`).
- [x] 1.6 One passing doctest in `csopesy_tests`, registered with CTest.
- [x] 1.7 `scripts/check.sh` and `scripts/check.ps1` running all seven gates in order.
- [ ] 1.8 `check` passes on every member's machine.

## Exit criteria

- [ ] _Done when `check` passes on every member's machine._
- [ ] All boxes above ticked and the user approves moving to Phase 2.
