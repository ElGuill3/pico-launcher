# Tasks: Style the DSteam Status Bar

## Final-State Reconciliation

This task record was reconciled after the final launcher was physically accepted, committed, and pushed. It records final evidence only; this reconciliation does not modify product source, tests, assets, `.atl`, Git history, remotes, RDD state, or hardware.

### Current Technical and Delivery State

| Field | Final state |
|---|---|
| Review budget | 800 changed lines |
| Delivery strategy | `auto-chain` |
| Chain strategy | `stacked-to-main` |
| Delivered commit sequence | `bb90392 feat(status): expose system status over RTC IPC` → `cf1feea feat(ui): add DSteam status bar` |
| Published state | `origin/feat/theme-launch-transition` at `cf1feea1bfcd9f143108f9ec51f1dc9f1328a65c` |
| Graphics allocation | 1,024 bytes: 256-byte rail, four 128-byte speaker frames, 128-byte battery slot, and 128-byte profile slot |
| Runtime graphics budget | At most 16 status OAM entries, 2,560 OBJ-VRAM bytes, and four palette rows |
| Final automated evidence | Focused C++ PASS; Python asset suite 5/5 PASS; Python discovery 13/13 PASS; `git diff --check` PASS; BlocksDS v1.16.0 build PASS; exact candidate source/object attribution completed |
| Physical acceptance | User physically tested the final launcher and explicitly accepted it: `Sip, ya esta perfecto, eso seria todo`. |

The final accepted presentation includes a horizontal battery, dynamic speaker, date/time, a square symmetric 16x16 profile tile with a lighter background, and the nickname shifted 2px left.

## Review Workload Forecast — Historical Delivery Record

| Field | Value |
|---|---|
| Estimated authored delta at planning | 436 lines (additions + deletions; PNG/generated output excluded) |
| 400-line budget risk | High |
| Chained PRs recommended | Yes |
| Delivery decision | Resolved automatically as `auto-chain` with `stacked-to-main` |
| Final delivery | Two coherent commits were physically accepted and pushed; this reconciliation creates no PR, commit, or remote update. |

Decision needed before apply: No
Chained PRs recommended: Yes
Chain strategy: stacked-to-main
400-line budget risk: High

## Historical Original Work (completed; do not reopen)

- [x] 1.1–1.3 Original RED contracts, pure mapping, assets, allocation fallback, OBJ rail, wiring, host/asset/diff/container verification.
- [x] 1.4 Original physical DSi XL acceptance is resolved by the final iterative physical acceptance of the launcher. The superseded intermediate acceptance record is not treated as a separate release gate.

## Corrective Work Unit — Strict TDD History

### Phase 5: RED Contracts
- [x] 5.1 Add RED tests in `tests/status_bar_format_tests.cpp` for source colors and RGB555 (`#3f4854`→`0x2928`, `#ffffff`→`0x7fff`, `#6fdc50`→`0x2b6d`, `#66c0f4`→`0x7aec`), opaque rail index through `y=15`, and no border at `y=15`.
- [x] 5.2 Add RED tests for white speaker/date-time, white battery outline/green states, blue nickname/avatar/right edge, exact disjoint layout, and unchanged `y>=16` geometry.
- [x] 5.3 Add RED tests for DSi/NTR truthful text fallback with no graphics write, profile glyph/ellipsis stability, and the final OAM/VRAM/graphics budgets; run and record the failing host command before production edits.

### Phase 6: GREEN Presentation
- [x] 6.1 Replace minimum `statusSpeaker.*` and `statusBattery.*` assets and add dedicated `arm9/gfx/statusProfile.png/.grit` 16x16 asset; retain transparent index and required frame ordering.
- [x] 6.2 Update `StatusBarFormat.h` with fixed palette/quantization, no-edge rail, exact profile layout, truthful fallback strings, and final graphics budgets.
- [x] 6.3 Update `RomBrowserTopScreenView.{h,cpp}` for fixed colors, revised profile group, fallback battery text, and safe allocation; preserve IPC, polling, acquisition, browser behavior, `y>=16`, and unrelated modifications.

### Phase 7: Automated Verification
- [x] 7.1 Record final focused C++ PASS, Python asset 5/5 PASS, Python discovery 13/13 PASS, `git diff --check` PASS, BlocksDS v1.16.0 build PASS, and exact candidate source/object attribution.

### Phase 8: Physical Release Gate

Audit note: the original planned exhaustive matrix covered light/dark/custom themes; DSi 0..4 × charging; NTR 3/4; volume 0/31/invalid; short/10-glyph/overlong/unsupported names; and every refresh. Those individual matrix cells were **not** separately recorded and are not claimed as executed.

- [x] 8.1 Record the actual final iterative real-hardware acceptance: the user physically tested the final launcher and accepted its final presentation. The acceptance covered the final horizontal battery, dynamic speaker, date/time, square symmetric 16x16 profile tile with lighter background, and nickname shifted 2px left; it is not an assertion that the original exhaustive matrix was individually executed.
- [x] 8.2 Accept the final physical outcome. No corrective reversal was required; IPC, polling, acquisition, browser behavior, and unrelated files remain unchanged.

Threat matrix: N/A; no routing, shell, subprocess, VCS, executable, or process-integration boundary applies.

## Completion Count

11/11 visible checklist entries are complete (covering 13 numbered task leaves). No implementation task remains unchecked.
