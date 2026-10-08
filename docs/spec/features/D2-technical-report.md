# D2 — Technical Report (`docs/report/TECHNICAL_REPORT.md`)

| Field            | Value                                                                                                      |
| ---------------- | ---------------------------------------------------------------------------------------------------------- |
| ID               | D2                                                                                                         |
| Summary          | The technical report, written as Markdown slides with presenter notes, to paste into a PPT later.          |
| Spec requirement | PPT (Technical Report): cover, video walkthrough, architectural diagram, code snippets, design discussion. |
| Phase            | 5 (tasks 5.2–5.4)                                                                                          |
| Depends on       | All features (for screenshots, snippets and the video); `docs/report/notes.md`                             |
| Files it touches | `docs/report/TECHNICAL_REPORT.md`, `docs/report/diagrams/`, `docs/report/notes.md`                         |

Written for any reader, from a first-year student to the instructor: every idea is explained in plain words first, with code only to illustrate it. It is never a line-by-line walk through the code.

Each `## Slide N — <title>` heading maps to one slide; under it go 3–5 short bullets (slide text) and a `> Notes:` block (what the presenter says). Diagrams live in `docs/report/diagrams/` as Mermaid source plus an exported PNG.

## Slides

| Slide | Content                                                                                                                              | Spec item             |
| ----- | ------------------------------------------------------------------------------------------------------------------------------------ | --------------------- |
| 1     | Cover: project title, course, group name and members, date                                                                           | Cover                 |
| 2     | Video walkthrough: link, plus a timestamped outline (boot, desktop, each app, PWR)                                                   | Video Walkthrough     |
| 3     | What we built, in one picture: a screenshot of the desktop with labels                                                               | Design discussion     |
| 4     | Key idea: immediate-mode UI, explained with an analogy (redrawing a whiteboard every frame vs. moving sticky notes)                  | Design discussion     |
| 5     | Architectural diagram: state flow + per-frame layer order, and the module map                                                        | Architectural Diagram |
| 6–9   | One slide per feature (Desktop, Taskbar, Task Manager, Terminal + File Explorer): what it does, the design choice, one short snippet | Code Snippets         |
| 10    | Code quality: formatting, linting and tests, and why they were automated                                                             | Design discussion     |
| 11    | Challenges and trade-offs (e.g. layering, resizing, keeping UI and data apart)                                                       | Design discussion     |
| 12    | Conclusion and what we'd add next                                                                                                    | Design discussion     |

## Rules for code snippets

- 5–8 snippets total, each ≤ 15 lines, trimmed to the idea being shown (`// ...` for omitted parts).
- Each one has a one-line caption above (what it does) and one or two sentences below (why it was done this way).
- Pick snippets that show a design decision: the frame loop, layer order, `AppWindow` base class, the taskbar toggle logic, the Processes table, the terminal command dispatcher.

**Writing rules:** short sentences; define each term the first time (immediate mode, compositor, frame, draw list); a glossary on the last slide notes if needed.

## Acceptance criteria

- No checklist boxes in the spec beyond the slide table above. Each slide row is present with 3–5 bullets and a `> Notes:` block, and the snippet rules are met.

## Unit tests to write

- None. Formatting is checked by gate 3 (Prettier on `docs/**/*.md`).
