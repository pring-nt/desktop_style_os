# Design notes

One line per design decision, for the Technical Report.

- 2026-10-08: Prettier pinned to 3.9.9 (latest stable at setup), run as `bunx prettier@3.9.9`.
- 2026-10-08: Acceptance criteria shared by a feature group in the plan (e.g. Boot sequence for F01–F02) are listed in each feature file of that group; phase exit criteria list each criterion once.
- 2026-10-08: Unit test files are named `tests/<module>_test.cc` (Google convention), e.g. `tests/clock_test.cc`.
- 2026-10-08: `.gitattributes` forces LF line endings (`* text=auto eol=lf`) so format checks give the same result on Windows and Unix checkouts.
- 2026-10-08: LLVM pinned to major version 23 (23.1.3, newest stable on winget at setup).
