# 04 — Quality Gates and Deterministic Testing

One command, `scripts/check.sh` (or `scripts/check.ps1` on Windows), runs every check in a fixed order and gives the same pass/fail on any machine, because tool versions, configs, file lists and test inputs are all pinned.

## What makes it deterministic

- Pinned tools: clang-format and clang-tidy share one LLVM major version, chosen once during Phase 1 setup and recorded in CONTRIBUTING.md; Prettier is pinned to an exact version. The script checks versions first and fails with a clear message on a mismatch.
- Pinned config: `.clang-format`, `.clang-tidy`, `.prettierrc`, `.editorconfig` and `.gitattributes` (`* text=auto eol=lf`, so every checkout has LF line endings) are committed; no tool runs on defaults.
- Fixed file set: files come from `git ls-files` (sorted), always excluding `third_party/` and `build/`.
- No hidden inputs in tests: no wall clock, no randomness, no window or GPU. `Clock` takes an injected time point; `DummyProcessTable` is a fixed list (any optional jitter uses a fixed-seed generator written by us, since `std::` distributions differ across standard libraries).
- Warnings are errors in the `ci` preset, so a warning can't pass on one machine and fail on another.

## Gates, in order (the script stops at the first failure)

| #   | Gate                       | Command                                                                                                                                      | Fails when                                                                    |
| --- | -------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------------------------------------- |
| 1   | Tool versions              | `clang-format --version`, `clang-tidy --version`, `bunx prettier --version`                                                                  | A tool is missing from PATH or the wrong version                              |
| 2   | C++ formatting             | `clang-format --dry-run --Werror` on `src/` and `tests/`                                                                                     | Any file differs from the style                                               |
| 3   | Markdown formatting        | `bunx prettier@<pinned> --check "docs/**/*.md"`                                                                                              | Spec or report Markdown is not formatted                                      |
| 4   | Configure + build          | `cmake --preset ci && cmake --build --preset ci`                                                                                             | Any compiler warning or error                                                 |
| 5   | Unit tests                 | `ctest --preset ci --output-on-failure`                                                                                                      | Any doctest case fails                                                        |
| 6   | Static analysis            | `clang-tidy -p build/ci` on `src/` and `tests/`                                                                                              | Any enabled check fires (`WarningsAsErrors: '*'`)                             |
| 7   | Layering and header guards | Search `src/data/` and the logic files for `imgui.h` / GL includes; check every header in `src/` for its `CSOPESY_SRC_<DIR>_<FILE>_H_` guard | UI headers leak into testable logic, or a header guard is missing or misnamed |

## Config baselines

- `.clang-format`: `BasedOnStyle: Google` with no overrides except `IncludeBlocks: Regroup` and `IncludeCategories` matching the Google include order (related header, C system, C++ standard, third-party, project).
- `.clang-tidy`: enable `google-*`, `bugprone-*`, `cppcoreguidelines-*`, `modernize-*`, `performance-*`, `readability-*`, `misc-*`; `readability-identifier-naming` configured to the Google naming table above, with snake_case methods exempt so accessors such as `is_open()` pass, and `CSOPESY_SRC_..._H_` header-guard macros exempt from the macro rule (gate 7 checks them). Disable only with a written reason, e.g. `modernize-use-trailing-return-type` (conflicts with Google style) and `cppcoreguidelines-pro-type-vararg` (ImGui's `Text()` is variadic by design), `readability-identifier-length` (Google style allows short names such as `i` and `dt`) and `llvm-header-guard` (builds the expected guard from each machine's absolute path; gate 7 checks guards instead). `HeaderFilterRegex` limited to `src/`.
- `.prettierrc`: `proseWrap: "preserve"`, `printWidth: 100`, so diffs on the report stay readable.

## Unit tests (doctest, `tests/`)

| Area                | Example cases                                                                                       |
| ------------------- | --------------------------------------------------------------------------------------------------- |
| `Clock`             | Fixed time point formats as `Thursday, Oct 08, 2026 \| 07:43 PM`; midnight and noon edge cases      |
| `StateMachine`      | BIOS → Splash after its duration; Splash → Desktop; PWR confirm → Shutdown; Cancel stays on Desktop |
| `WindowManager`     | Taskbar click toggles open → focus → minimize → restore; running flags match open windows           |
| `TerminalCommands`  | Each command's exact output lines; unknown command message; `cls` clears; history order             |
| `DummyProcessTable` | Row count, totals equal row sums, CPU total capped at 100%                                          |

UI drawing itself is verified by the manual test checklist in the Milestones section (see [phases/phase-5-report-and-submission.md](phases/phase-5-report-and-submission.md)); the gates cover everything that can be checked without a screen.

**Optional:** a git pre-commit hook that runs gates 2–3, and a GitHub Actions workflow that runs the same script on `windows-latest` and `ubuntu-latest`.

## Tool setup (`CONTRIBUTING.md`)

Not everyone has clang tools on PATH yet, so `CONTRIBUTING.md` walks each member through setup and records the pinned versions everyone must match.

| Tool                            | Windows                                                                  | macOS                                                          | Linux                                       |
| ------------------------------- | ------------------------------------------------------------------------ | -------------------------------------------------------------- | ------------------------------------------- |
| LLVM (clang-format, clang-tidy) | `winget install LLVM.LLVM`, then add `C:\Program Files\LLVM\bin` to PATH | `brew install llvm@<N>`, then add its `bin` to PATH (keg-only) | `apt.llvm.org` script for version `<N>`     |
| CMake ≥ 3.24 + Ninja            | `winget install Kitware.CMake Ninja-build.Ninja`                         | `brew install cmake ninja`                                     | Package manager                             |
| bun (for Prettier)              | `winget install Oven-sh.Bun`                                             | `brew install oven-sh/bun/bun`                                 | `curl -fsSL https://bun.sh/install \| bash` |

- During Phase 1, pick the newest stable LLVM major version, write it into `CONTRIBUTING.md` as `<N>`, and have every member install that exact major version.
- A "Verify your setup" step: run `clang-format --version`, `clang-tidy --version`, `cmake --version`, `ninja --version`, `bun --version`, then `scripts/check`.
- It also documents the git conventions (Conventional Commits, `feat/…` / `fix/…` branches) and how to open a PR for review.
