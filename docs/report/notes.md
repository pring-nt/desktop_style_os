# Design notes

One line per design decision, for the Technical Report.

- 2026-10-08: Prettier pinned to 3.9.9 (latest stable at setup), run as `bunx prettier@3.9.9`.
- 2026-10-08: Acceptance criteria shared by a feature group in the plan (e.g. Boot sequence for F01–F02) are listed in each feature file of that group; phase exit criteria list each criterion once.
- 2026-10-08: Unit test files are named `tests/<module>_test.cc` (Google convention), e.g. `tests/clock_test.cc`.
- 2026-10-08: `.gitattributes` forces LF line endings (`* text=auto eol=lf`) so format checks give the same result on Windows and Unix checkouts.
- 2026-10-08: LLVM pinned to major version 23 (23.1.3, newest stable on winget at setup).
- 2026-10-08: clang-tidy `llvm-header-guard` dropped: it derives the expected guard from the absolute file path, so it cannot enforce `CSOPESY_SRC_<DIR>_<FILE>_H_`. Gate 7 of `scripts/check` checks guards instead.
- 2026-10-08: clang-tidy also disables `readability-identifier-length` (Google allows `i`, `dt`) and exempts snake_case methods from PascalCase so accessors like `is_open()` pass.
- 2026-10-08: Include order puts `<GLFW/glfw3.h>` before `<glad/gl.h>`, so CMake must define `GLFW_INCLUDE_NONE`.
- 2026-10-08: Dependencies pinned as release archives with SHA-256 hashes: GLFW 3.4 (as the spec fixes it, although 3.5.1 exists), Dear ImGui v1.92.9b, doctest v2.5.3. Vendored: glad2 2.0.8 (GL 3.3 core, no extensions) and stb_image v2.30.
- 2026-10-08: ASan + UBSan only run outside Windows: MinGW GCC has no sanitizer runtime. The `release` preset skips tests; `ci` builds them.
- 2026-10-08: Third-party include dirs are made SYSTEM via `INTERFACE_SYSTEM_INCLUDE_DIRECTORIES`, because the `SYSTEM` option of `FetchContent_Declare` needs CMake 3.25 and the floor is 3.24.
- 2026-10-08: The Phase 1 bootstrap lives in `src/main.cc` (RAII wrappers for GLFW, the window and ImGui) and moves into `App` in task 2.6. `csopesy_core` is added once there are sources besides `main.cc`.
- 2026-10-08: The ImGui demo window is behind the `CSOPESY_IMGUI_DEMO` CMake option (ON in the `debug` preset) and only compiles into Debug builds. `imgui.ini` is disabled (`IniFilename = nullptr`) so no layout state persists between runs.
- 2026-10-08: MinGW builds link libgcc/libstdc++ statically and copy `libwinpthread-1.dll` next to each exe (`csopesy_use_portable_runtime`). This also stops exes from loading a mismatched runtime from another MinGW on PATH, such as Git Bash's. A fully static link fails with the MinGW bundled with CLion.
- 2026-10-08: Tests are registered per test case with `doctest_discover_tests`, so `ctest -R <name>` runs a single case. The Phase 1 test is a smoke test of the harness; real tests start with `Clock` in task 2.1.
- 2026-10-08: `scripts/check.*` list files with `git ls-files --cached --others --exclude-standard`, so new files that are not yet committed are checked too. Gate 7 treats `src/data/`, `src/core/clock.*` and `src/core/state_machine.*` as testable logic.
- 2026-10-08: On Windows the check runs as `powershell -ExecutionPolicy Bypass -File scripts/check.ps1` instead of changing the machine's execution policy.
- 2026-10-08: `Clock` splits time-zone conversion (`ToLocalTime`, via `localtime_s` / `localtime_r`) from formatting (`FormatDateTime`, `std::put_time` with the classic locale). Formatting tests build `std::tm` fields directly, so they pass in any time zone; the injection test compares against the same conversion.
- 2026-10-08: `csopesy_core` starts with `src/core/clock.cc`; `src/` is its public include root, so project includes read `"core/clock.h"`.
- 2026-10-08: `StateMachine` holds state, timers and the PWR confirm step (`RequestShutdown` / `CancelShutdown` / `ConfirmShutdown`) but draws nothing; `App` picks what to render for each state, so the logic stays unit-testable. Default durations: BIOS 4 s, splash 2.5 s, shutdown 1 s.
- 2026-10-08: Fonts use the two fonts built into Dear ImGui (MIT): ProggyForever, the scalable default, for the shell and ProggyClean, a pixel font, for the boot screens. No font files or extra licenses to ship; a sans font such as Roboto can be added later as polish.
- 2026-10-08: `Theme` is in `csopesy_core`, which now links `imgui`. Its test runs on a bare ImGui context with no window or GPU.
