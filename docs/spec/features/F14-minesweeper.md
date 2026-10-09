# F14 — Minesweeper (optional extra)

| Field            | Value                                                                                                                                                                                           |
| ---------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ID               | F14                                                                                                                                                                                             |
| Summary          | A classic Minesweeper game as a fourth app window, pinned to the taskbar.                                                                                                                       |
| Spec requirement | None (team request, Phase 4 polish).                                                                                                                                                            |
| Phase            | 4 (task 4.8)                                                                                                                                                                                    |
| Depends on       | `AppWindow` base and `WindowManager` (Phase 2), taskbar app buttons (F07)                                                                                                                       |
| Files it touches | `src/data/minesweeper_board.h`, `src/data/minesweeper_board.cc`, `src/data/random.h`, `src/apps/minesweeper.h`, `src/apps/minesweeper.cc`, `src/shell/taskbar.*`, `tests/minesweeper_*_test.cc` |

## Requirements

- Opens from its own taskbar button (a mine icon) with the same open, minimize, restore and close behavior as the other apps.
- Three difficulties as in Windows: Beginner 9 × 9 with 10 mines, Intermediate 16 × 16 with 40 mines, Expert 30 × 16 with 99 mines.
- Left click reveals a cell; an empty cell reveals its neighbors in a flood fill. Right click places or removes a flag. Clicking a revealed number whose flags match it reveals the remaining neighbors (chording).
- The first reveal is always safe: mines are placed after it, away from the clicked cell and its neighbors.
- A status bar shows mines left (mines minus flags), a face button that starts a new game, and the elapsed time in seconds.
- Revealing a mine loses (all mines shown, wrong flags crossed out); revealing every safe cell wins (remaining mines flagged).
- The board logic lives in `data/` with no ImGui or GL includes. Mine placement uses a fixed-seed generator written by us, so tests are deterministic.

## Acceptance criteria

- [x] Opens from its taskbar button and behaves like the other app windows.
- [x] The first click never hits a mine, empty areas flood-reveal, and flags block reveals.
- [x] Winning and losing are detected and shown; the face button starts a new game.

## Unit tests to write

- Mine count matches the difficulty, and the first revealed cell and its neighbors are mine-free.
- Neighbor counts, flood fill, flag toggling, chording, win and loss.
- The same seed places mines in the same cells.
