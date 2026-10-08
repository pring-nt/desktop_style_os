# 02 — Build System (CMake)

One CMake setup builds identically on every member's machine: pinned dependencies, presets instead of hand-typed flags, strict warnings on our code only, and tests and lint wired in as targets.

## Rules

- `cmake_minimum_required(VERSION 3.24)`; C++20 with `CMAKE_CXX_STANDARD_REQUIRED ON` and `CMAKE_CXX_EXTENSIONS OFF`.
- `CMAKE_EXPORT_COMPILE_COMMANDS ON` so clang-tidy and editors see the real flags.
- Modern, target-based CMake only: `target_*` commands, never global `include_directories`, `add_definitions` or `CMAKE_CXX_FLAGS` edits.
- No in-source builds: configure fails if the build dir equals the source dir.
- Dependencies: GLFW, Dear ImGui and doctest via `FetchContent` pinned to exact release tags in `cmake/Dependencies.cmake`; glad and stb_image vendored unmodified in `third_party/`. Never "latest" or a branch name.
- Dear ImGui has no CMake file of its own, so we define an `imgui` static library from its core sources plus `imgui_impl_glfw` and `imgui_impl_opengl3`.
- Third-party include dirs are marked `SYSTEM` so their warnings never fail our build or lint.
- Post-build step copies `assets/` next to the executable (`$<TARGET_FILE_DIR:csopesy_os>`).

## Targets

| Target                    | Type       | Contents                                                   |
| ------------------------- | ---------- | ---------------------------------------------------------- |
| `imgui`                   | static lib | Dear ImGui core + GLFW/OpenGL3 backends                    |
| `csopesy_core`            | static lib | Everything in `src/` except `main.cc`                      |
| `csopesy_os`              | executable | `main.cc`, links `csopesy_core`                            |
| `csopesy_tests`           | executable | doctest tests, links `csopesy_core`; registered with CTest |
| `format` / `format-check` | custom     | Apply / verify clang-format                                |
| `tidy`                    | custom     | Run clang-tidy on `src/` and `tests/`                      |

## Warnings (`cmake/CompilerWarnings.cmake`, applied to our targets only)

- MSVC: `/W4 /permissive- /WX`
- GCC / Clang: `-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wold-style-cast -Wnon-virtual-dtor -Werror`
- Toggle: `CSOPESY_WARNINGS_AS_ERRORS` (default ON).

## Presets (`CMakePresets.json`, Ninja generator, output in `build/<preset>/`)

| Preset    | Purpose                                                           |
| --------- | ----------------------------------------------------------------- |
| `debug`   | Day-to-day work; debug symbols; ASan + UBSan on GCC/Clang         |
| `release` | Optimized build for the demo and submission                       |
| `ci`      | Release + tests + warnings as errors; what `scripts/check.*` uses |

Standard commands: `cmake --preset debug`, `cmake --build --preset debug`, `ctest --preset debug`.
