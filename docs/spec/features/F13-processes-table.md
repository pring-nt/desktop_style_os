# F13 — Processes table (required)

| Field            | Value                                                                                                                                |
| ---------------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| ID               | F13                                                                                                                                  |
| Summary          | The Task Manager's table of ~20 fixed fake processes with CPU and memory values.                                                     |
| Spec requirement | Task Manager: placeholder Processes table with CPU and memory, dummy values.                                                         |
| Phase            | 3 (tasks 3.6, 3.7)                                                                                                                   |
| Depends on       | Phase 2 (data layer); F12 window (3.7) for drawing                                                                                   |
| Files it touches | `src/data/dummy_process_table.h`, `src/data/dummy_process_table.cc`, `src/apps/task_manager.cc`, `tests/dummy_process_table_test.cc` |

## Requirements

| Column | Example             |
| ------ | ------------------- |
| Name   | `csopesy_shell.exe` |
| Status | Running             |
| CPU    | 3.4%                |
| Memory | 128.6 MB            |

- Built with `BeginTable` (`RowBg | BordersInnerV | ScrollY`, frozen header row); numbers right-aligned.
- Rows come from `DummyProcessTable`: a fixed list of ~20 fake processes defined in one place, so the screen is identical on every run and easy to test.
- Totals in the header are computed from the rows (CPU capped at 100%).

**Optional polish (only if time allows):** small random jitter on values each second, column sorting, an "End task" button.

## Acceptance criteria (Task Manager, shared with F12)

- [ ] Table shows each process with its CPU and memory usage, all dummy values.
- [ ] Header totals match the sum of the rows.

## Unit tests to write

- `DummyProcessTable`: row count, totals equal row sums, CPU total capped at 100%.
- Any optional jitter uses a fixed-seed generator written by us, since `std::` distributions differ across standard libraries.
