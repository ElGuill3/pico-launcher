# Proposal: Compose Status Text on Theme Backgrounds

## Intent

Hardware accepts theme-owned backgrounds, but binary OBJ text loses stable antialiased edges. Game text remains stable only over known opaque backing. Compose status text into supported representations without replacing theme pixels.

## Scope

### In Scope
- Compose date/time, NTR `HIGH`/`LOW`, and nickname using full NFT2 coverage; user-approved readability geometry places nickname at `[190,256)`.
- Preserve custom RGB555 pixels outside glyph coverage and restore from pristine before text changes.
- Preserve tiled material BG0 under its verified uniform top-strip invariant; fail closed to truthful binary OBJ fallback when unsupported.
- Define lifecycle, allocation failure, fallback, cover precedence, and VBlank safety.
- Require host contracts before integration, build evidence, and an explicitly scoped physical acceptance record.

### Out of Scope
- Profile placeholder, fixed rail, speaker/battery changes, or material RGB555 conversion.
- Theme metadata, animation, game-renderer redesign, IPC/polling/status changes, or archive edits.
- Sampled/fixed-color OBJ, threshold-first rendering, or a separate overlay BG.

## Capabilities

### New Capabilities
None.

### Modified Capabilities
- `dsteam-status-bar-presentation`: Replace rail/profile requirements with theme-owned composition, truthful fallback, and updated acceptance evidence via delta spec.

## Approach

Use a narrow contract owned by the active background. Custom themes rebuild a bounded RGB555 strip from pristine pixels. Material retains BG0 and composes only while its top strip is uniform. BG3/WIN0 covers retain precedence. Unsupported or unavailable composition and allocation failure restore pristine state and select truthful binary OBJ text; stale text is forbidden. Publish only during safe VBlank and validate timing physically.

## Affected Areas

| Area | Impact | Description |
|------|--------|-------------|
| `arm9/source/themes/` | Modified | Contract and backend lifecycle |
| `arm9/source/App.cpp` | Modified | Connect background and status owner |
| `arm9/source/romBrowser/views/` | Modified | Lifecycle and fallback selection |
| `tests/` | Modified | Host-first rendering and failure contracts |

## Risks

| Risk | Likelihood | Mitigation |
|------|------------|------------|
| VBlank work misses timing | Med | Measure with covers active; fail back safely |
| Material invariant drifts | Med | Executable guard and closed fallback |
| Partial failure corrupts theme pixels | Low | Pristine restoration and bounded writes |

## Rollback Plan

Disable composition, restore pristine pixels, and retain truthful binary OBJ text. Revert only change-owned files; preserve the status-bar baseline, unrelated `.atl` drift, and OpenSpec scaffolding.

## Dependencies

- Uncommitted status-bar follow-up baseline and NFT2 coverage path.

## Success Criteria

- [x] Host contracts prove blending, clipping, restoration, lifecycle/failure behavior, and material invariant before integration.
- [x] Writes stay within glyph/strip bounds; cover precedence and truthful fallback remain intact.
- [x] The final observed configuration is physically accepted; the original 5×4 matrix remains **NOT EXECUTED** and **NOT PASSED**.
