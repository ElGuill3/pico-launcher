# Tasks: Compose Status Text on Theme Backgrounds

## Review Workload Forecast

| Field | Value |
|-------|-------|
| Estimated changed lines | 900–1,200 across 10 files |
| 400-line budget risk | High |
| Chained PRs recommended | Yes |
| Suggested split | PR 1 compositor/renderer; PR 2 background backends; PR 3 integration/evidence |
| Delivery strategy | auto-chain |
| Chain strategy | stacked-to-main |

Decision needed before apply: No
Chained PRs recommended: Yes
Chain strategy: stacked-to-main
400-line budget risk: High

### Suggested Work Units

| Unit | Goal | Likely PR | Focused test command | Runtime harness | Rollback boundary |
|------|------|-----------|----------------------|-----------------|-------------------|
| 1 | Pure compositor and NFT2 coverage contracts | PR 1 | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp tests/status_bar_format_tests.cpp -o /tmp/status-tests && /tmp/status-tests` | N/A: host-pure renderer/compositor | Revert `StatusStripComposition.h`, `Nft2CoverageRenderer.*`, and their tests |
| 2 | Custom RGB555 and guarded material composition | PR 2 | Same focused host command plus `python3 -m unittest discover -s tests -p 'test_*.py'` | N/A: backend contracts require BlocksDS/DSi; host tests cover guards | Revert `IThemeBackground.h`, `CustomSubBackground.*`, `MaterialSubBackground.*` |
| 3 | View/App wiring and acceptance evidence | PR 3 | `make` (in `skylyrac/blocksds:slim-v1.16.0`) | Physical DSi: covers active, theme/load/mode/minute/NTR/nickname transitions | Revert `App.cpp` and `RomBrowserTopScreenView.*`; retain prior backend slices |

**Orchestrator-owned prerequisite:** acquire and settle native toolchain/container and physical DSi access, including covered timing capture, before delegating. Phase agents MUST NOT improvise hardware/SD/native acquisition commands.

## Phase 1: Pure Contracts (PR 1, RED → GREEN)

- [x] 1.1 RED: extend `tests/status_bar_format_tests.cpp` for geometry, fallback semantics, nickname `[186,236)`, ellipsis/unsupported glyphs, and unchanged speaker/battery/NTR behavior.
- [x] 1.2 RED: create `tests/status_strip_compositor_tests.cpp` covering coverage 0/1/8/15 rounding, bit 15, half-open clipping/canaries, pristine restoration, dirty/results, and invalid spans.
- [x] 1.3 GREEN: add `arm9/source/themes/background/StatusStripComposition.h` with request/result modes and bounded RGB555 blending; then add `arm9/source/gui/views/Nft2CoverageRenderer.{h,cpp}` without changing generic label quantization.

## Phase 2: Background Backends (PR 2, RED → GREEN)

- [x] 2.1 RED: add host cases for exact 98,304-byte custom input, allocation/load failure, rebuild-from-pristine, material uniformity, exact palette endpoint, and unsupported truth-table fallback.
- [x] 2.2 GREEN: modify `arm9/source/themes/background/IThemeBackground.h` with synchronous prepare/restore and VBlank-safe publication contracts; implement validated 8 KiB strips, lifecycle release, and one dirty-strip upload in `CustomSubBackground.{h,cpp}`.
- [x] 2.3 GREEN: implement map/tile top-strip guard, RAM palette endpoint, and fail-closed behavior in `MaterialSubBackground.{h,cpp}`; preserve BG0 and cover precedence.

## Phase 3: Integration and Evidence (PR 3, RED → GREEN)

- [x] 3.1 RED: add mode/lifecycle/cover-order assertions to `tests/status_bar_format_tests.cpp`, including reconstruction, theme/mode changes, stale-text prevention, and truthful DSi charging/segments.
- [x] 3.2 GREEN: wire background injection at all three reconstruction sites in `arm9/source/App.cpp`; update `RomBrowserTopScreenView.{h,cpp}` for masks, atomic mode visibility, direct palette, fallback, and background-before-cover VBlank order.
- [x] 3.3 Record focused checks, build/install evidence, and maintainer-selected limited physical acceptance for the final observed layout; preserve the original 5×4 matrix as **NOT EXECUTED** and **NOT PASSED**, with no unobserved coverage claim.

### Remediation Work Unit: physical-layout-remediation

- [x] RED → GREEN: Historical Medium10 metric step; superseded by completed Medium9 remediation, which verifies final collision-free nickname `[190,256)` geometry.
- [x] RED → GREEN: Historical atomic full-strip DMA step; superseded by completed bounded dirty-row publication with an ARM9 DMA source cache flush.
- [x] Record the physical-layout-remediation matrix limitation as **NOT EXECUTED** and **NOT PASSED**; no separate physical-matrix task remains.

### Remediation Work Unit: vblank-starvation-remediation

- [x] RED → GREEN: revert only the full waited 8 KiB atomic publication slice to bounded dirty-row publication; remove its atomic contract, flush each composed DMA source row, and derive the 2176-byte status OBJ allocation budget from rounded label buffers plus status graphics.
- [x] Record the vblank-starvation-remediation matrix limitation as **NOT EXECUTED** and **NOT PASSED**; no separate physical-matrix task remains.

### Remediation Work Unit: coverage-buffer-overflow-remediation

- [x] RED → GREEN: derive shared coverage capacity from `NicknameTextWidth * StatusHeight`; prove 64px center, 24px NTR, and 66px nickname renderer paths preserve surrounding canaries.
- [x] Record the coverage-buffer-overflow-remediation matrix limitation as **NOT EXECUTED** and **NOT PASSED**; no separate physical-matrix task remains.

### Remediation Work Unit: profile-gap-eight

- [x] RED → GREEN: final accepted geometry uses `StatusNicknameRight=256` and `StatusBatteryGap=18`, preserving battery coordinates from the 4px-left candidate with DSi/NTR clamps for maximum and overlong nicknames; the configured 18px gap does not assert every clamped effective gap.
- [x] Record the profile-gap-eight matrix limitation as **NOT EXECUTED** and **NOT PASSED**; no separate physical-matrix task remains.

## Phase 4: Preservation Check

- [x] 4.1 Confirm diff excludes `.atl`, archives, assets, current capability specs, unrelated status-bar baseline/drift, commits, and PR/hardware changes; verify rollback by forcing `Unsupported` and restoring pristine.
