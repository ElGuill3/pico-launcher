# Proposal: Style the DSteam Status Bar

## Intent

Correct the physically rejected DSi XL presentation with a fixed Steam Deck-inspired branded rail while preserving the validated status pipeline and ROM-browser workflow.

## Scope

### In Scope
- Render a fixed 256x16 `#3f4854` rail with no bottom edge, keyline, or border.
- Show a white speaker; centered white date/time and a battery with white outline, exact `#6fdc50` fill, truthful segments/charging, and no percentage.
- Place a blue nickname and compact profile-picture placeholder at the far right, ending with a blue right edge/margin rather than a bracket-like keyline.
- Preserve left/center/right composition, collision-free fit, every validated status semantic, and theme content below `y=15`.

### Out of Scope
- Acquisition, IPC, polling, payload, fallback, data semantics, or ROM-browser behavior changes.
- Theme-derived rail/status colors, quick settings, percentages, network indicators, BG rewrites, or unrelated redesign.

## Capabilities

### New Capabilities
- `dsteam-status-bar-presentation`: Fixed branded styling, compact profile treatment, collision-free composition, and truthful platform states.

### Modified Capabilities
- None; `openspec/specs/` has no existing capability specifications.

## Approach

Retain the shared OBJ rail, existing font, compact assets, ellipsis, and pure visual-state mapping. Replace theme token resolution with fixed branded palette values, remove edge/keyline rendering, and revise the right group for blue nickname plus far-right profile placeholder. Leave acquisition and both theme background engines unchanged; theme compatibility means the fixed rail remains readable over every background.

## Affected Areas

| Area | Impact | Description |
|------|--------|-------------|
| `arm9/source/romBrowser/views/RomBrowserTopScreenView.{h,cpp}` | Modified | Fixed palette, border removal, profile layout |
| `arm9/source/romBrowser/views/StatusBarFormat.h` | Modified | Fixed visual tokens and layout contract |
| `arm9/gfx/status{Speaker,Battery}.*` | Modified | White speaker; white-outline/green-fill battery; profile placeholder |
| `tests/status_bar_format_tests.cpp`, `tests/test_status_bar_assets.py` | Modified | Exact colors, geometry, states, and budgets |

## Risks

| Risk | Likelihood | Mitigation |
|------|------------|------------|
| Fixed palette is unclear on a theme | Medium | Opaque rail isolates status contrast; inspect representative backgrounds on hardware |
| Right profile group overlaps center | Medium | Prove half-open bounds at maximum content widths |
| Hardware differs from the reference | Medium | Make physical DSi XL acceptance an exact release gate |

## Rollback Plan

Revert the corrective presentation delta to the currently installed implementation; if the branded direction remains unacceptable, revert the complete status-bar view/helper/assets/tests set to validated plain-text labels. No data, IPC, or firmware migration is required.

## Dependencies

- Existing status payload/fallbacks, OBJ managers, font, GRIT pipeline, and physical DSi XL review.

## Success Criteria

- [ ] Physical DSi XL shows a contiguous 256x16 `#3f4854` rail with no bottom line, border, or bracket-like profile keyline.
- [ ] Speaker and date/time are white; battery has a white outline and exact `#6fdc50` fill, truthful segments/charging, and no percentage.
- [ ] Blue nickname and compact profile placeholder occupy the far-right area with a blue trailing edge/margin.
- [ ] Volume-left, centered date/time+battery, and profile-right groups fit 256x16 without overlap or flicker across maximum states.
- [ ] Fixed branded colors remain readable over representative backgrounds; `y>=16`, acquisition, IPC, polling, fallbacks, and browser behavior are unchanged.
