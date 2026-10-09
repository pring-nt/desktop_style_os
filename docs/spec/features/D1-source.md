# D1 — SOURCE

| Field            | Value                                                                                         |
| ---------------- | --------------------------------------------------------------------------------------------- |
| ID               | D1                                                                                            |
| Summary          | The source code, submitted as a GitHub repository link, with a root `README.txt`.             |
| Spec requirement | SOURCE: source code, README.txt with names, run instructions and entry file (or GitHub link). |
| Phase            | 5 (task 5.1)                                                                                  |
| Depends on       | Phase 4; `CONTRIBUTING.md` (1.2)                                                              |
| Files it touches | `README.txt`, `.gitignore`                                                                    |

Two things are submitted: the SOURCE (as a GitHub link) and the technical report, which we write as Markdown in `docs/report/` and paste into slides later instead of authoring a PPT directly.

## Requirements

- Submitted as a **GitHub repository link**. The repo holds all source, CMake files, assets and the quality-gate config (`.clang-format`, `.clang-tidy`, `.prettierrc`, `scripts/`).
- Never committed: `build/`, `.vs/`, binaries, IDE caches and `CLAUDE.md`; `.gitignore` enforces this.
- `README.txt` at the repo root containing:
  - Group member names.
  - Requirements: OS, compiler (e.g. MSVC 2022 / GCC 13 / Clang 17), CMake ≥ 3.24, Ninja.
  - Build and run steps, copy-pasteable: `cmake --preset release`, `cmake --build --preset release`, then the path of the executable.
  - Entry point: `src/main.cc`, which holds `int main()` and creates the `App` class (`src/core/app.cc`) that runs everything. (C++ has no "entry class"; this sentence answers the spec's question.)
  - A pointer to `CONTRIBUTING.md` for developer tooling and the check script.

## Acceptance criteria

- [ ] README steps, followed exactly on a machine without the dev setup, produce a running build.
- [x] `scripts/check` passes from a clean clone (all seven gates).

## Unit tests to write

- None.
