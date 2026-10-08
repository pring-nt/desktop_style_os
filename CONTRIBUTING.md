# Contributing

How to set up your machine, which tool versions everyone must match, and how we use git.

## Pinned versions

Every member installs these versions. `scripts/check` checks them first and fails with a clear message on a mismatch, so a check that passes on one machine passes on all of them.

| Tool                            | Version            | Notes                                  |
| ------------------------------- | ------------------ | -------------------------------------- |
| LLVM (clang-format, clang-tidy) | major **23**       | Same major version for both tools      |
| Prettier                        | **3.9.9** exactly  | Run through bun: `bunx prettier@3.9.9` |
| CMake                           | ≥ 3.24             |                                        |
| Ninja                           | any recent release | Generator used by every preset         |
| bun                             | any recent release | Only used to run Prettier              |
| C++ compiler                    | C++20 capable      | MSVC 2022, GCC 13+ or Clang 17+        |

## Tool setup

| Tool                            | Windows                                                                  | macOS                                                         | Linux                                       |
| ------------------------------- | ------------------------------------------------------------------------ | ------------------------------------------------------------- | ------------------------------------------- |
| LLVM (clang-format, clang-tidy) | `winget install LLVM.LLVM`, then add `C:\Program Files\LLVM\bin` to PATH | `brew install llvm@23`, then add its `bin` to PATH (keg-only) | `apt.llvm.org` script for version `23`      |
| CMake ≥ 3.24 + Ninja            | `winget install Kitware.CMake Ninja-build.Ninja`                         | `brew install cmake ninja`                                    | Package manager                             |
| bun (for Prettier)              | `winget install Oven-sh.Bun`                                             | `brew install oven-sh/bun/bun`                                | `curl -fsSL https://bun.sh/install \| bash` |

On Windows, add LLVM to your user PATH from PowerShell, then open a new terminal:

```powershell
[Environment]::SetEnvironmentVariable('Path', [Environment]::GetEnvironmentVariable('Path','User') + ';C:\Program Files\LLVM\bin', 'User')
```

On Linux, the `apt.llvm.org` script installs versioned binaries (`clang-format-23`, `clang-tidy-23`). Make sure the plain `clang-format` and `clang-tidy` names on PATH point to version 23, for example with `update-alternatives`.

## Verify your setup

Run each command in a new terminal and compare against the pinned versions above:

```
clang-format --version
clang-tidy --version
cmake --version
ninja --version
bun --version
```

Then run the full check from the repo root:

```
scripts/check.sh      # macOS / Linux / Git Bash
scripts/check.ps1     # Windows PowerShell
```

## Build and test

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Use the `release` preset for the demo build. `scripts/check` uses the `ci` preset.

## Git conventions

- Commit messages follow [Conventional Commits](https://www.conventionalcommits.org/): `feat: add taskbar running indicators`, `fix: keep clock in corner on resize`, `docs: …`, `test: …`, `chore: …`, `build: …`, `refactor: …`.
- Branches are named after the change: `feat/taskbar`, `fix/clock-format`, `docs/report-slides`.
- Text files use LF line endings on every OS (`.gitattributes`). Set your editor to LF for this repo.
- Never commit `build/`, `.vs/`, `.idea/`, binaries or `CLAUDE.md`; `.gitignore` covers them.

## Opening a pull request

1. Branch off `master`: `git switch -c feat/<name>`.
2. Commit in small, focused Conventional Commits.
3. Run `scripts/check` and make sure every gate passes. Never disable a lint check, loosen a warning or skip a test to get green; raise it with the team instead.
4. Push and open a PR against `master` (`gh pr create` or the GitHub web UI). Describe what changed and which spec feature (`F01`–`F13`, `D1`, `D2`) it covers.
5. Ask another member to review. Merge only after approval.
