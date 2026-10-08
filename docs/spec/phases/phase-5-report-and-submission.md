# Phase 5 — Report and submission

## Goal

`TECHNICAL_REPORT.md`, diagrams, video walkthrough, `README.txt`, resize and clean-machine testing, final `check` run.

## Feature IDs included

- [D1](../features/D1-source.md), [D2](../features/D2-technical-report.md)

## Task order

- [ ] 5.1 D1 `README.txt`.
- [ ] 5.2 D2 diagrams in `docs/report/diagrams/` (Mermaid source + exported PNG).
- [ ] 5.3 Video walkthrough.
- [ ] 5.4 D2 `docs/report/TECHNICAL_REPORT.md`.
- [ ] 5.5 Resize and clean-machine testing (test checklist below).
- [ ] 5.6 Final `check` run.

## Test checklist

- [ ] `scripts/check` passes from a clean clone (all seven gates).
- [ ] Cold launch shows BIOS → Splash → Desktop with no console errors or ImGui asserts in a debug build.
- [ ] Resize the window small and large: wallpaper, clock, taskbar and windows stay correctly placed.
- [ ] Open, minimize, restore, close each app from both the taskbar and its window.
- [ ] Open all three apps at once; overlap and focus order behave correctly.
- [ ] Terminal: every listed command works; Up/Down history works; unknown commands show the error line.
- [ ] PWR → Shut Down exits cleanly; Cancel returns to the desktop.
- [ ] Missing wallpaper file falls back to the gradient without crashing.
- [ ] README steps, followed exactly on a machine without the dev setup, produce a running build.

## Exit criteria

- [ ] All boxes above ticked and `scripts/check` green.
- [ ] Every row of the requirements traceability table in [README.md](../README.md) passes.
