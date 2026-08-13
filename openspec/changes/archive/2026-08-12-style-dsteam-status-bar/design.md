# Design: Correct the DSteam Status Bar

## Technical Approach

Correct only the rejected presentation already owned by `RomBrowserTopScreenView`. Keep its shared OBJ rail, `Medium7_5` labels, one aligned status block, pure mapping, refresh cadence, and VBlank uploads. Replace theme-derived tokens and the keyline with fixed branded colors and a compact profile asset. IPC, polling, acquisition, fallbacks, browser flow, and all pixels at `y>=16` remain unchanged.

## Architecture Decisions

| Option | Tradeoff | Decision and rationale |
|---|---|---|
| Fixed tokens | Not theme-customizable | Use rail `#3f4854`, white `#ffffff`, battery `#6fdc50`, and online blue `#66c0f4`; the opaque rail guarantees contrast and removes the RDD degenerate-theme ambiguity. |
| Dedicated 16x16 profile asset | Adds one tiny asset pair | Use a 12x12 visible placeholder inside one 16x16 OBJ. Reusing a 16x8 battery frame cannot produce a legible square; the existing 128-byte profile slot is unchanged. |
| Existing labels for allocation fallback | Rail/icons may be unavailable | Generalize the NTR label into a battery-state label; this preserves truth without another label, allocation, or write. |
| New renderer/configuration layer | More abstraction | Reject; the current view and pure helper are the bounded correction point. |

## Data Flow and Contracts

`SystemStatus -> PresentationState -> graphics-ready/layout/text/frame -> Draw -> VBlank`.

`StatusBarFormat.h` exposes fixed tokens, RGB555 quantization, frames, fallback text, and half-open bounds. Valid volume is `dsiMode && VOLUME_VALID`, clamped to 0..31; otherwise icon and number are absent. DSi maps levels to 0..4 segments and independent charging frames; NTR remains `HIGH` for level `>3`, otherwise `LOW`. On invalid status-block offset/address, draw no rail/icon/profile OAM and perform no status-graphics write; show DSi `0/4`..`4/4` plus `+` when charging, or NTR `HIGH`/`LOW` in the existing 24px battery label.

The exact layout is `[0,256)x[0,16)`. Every rail texel, including `y=15`, uses opaque index 1; no edge is generated. Left: speaker `[4,12)x[4,12)`, white number `[14,34)x[0,16)`. DSi center: date `[86,150)`, battery `[154,170)`, composite `[86,170)`. NTR: date `[73,137)`, battery `[141,157)`, text `[159,183)`, composite `[73,183)`. Text fallback: date `[82,146)`, battery state `[150,174)`. Right: blue nickname `[188,238)`, ellipsized within 50px with unsupported glyph `?`; 16x16 profile OBJ `[240,256)x[0,16)` contains a 12x12 blue-backed, white-silhouette placeholder `[242,254)x[2,14)` and blue trailing strip `[254,256)x[0,16)`. Thus left, widest center, and profile content are disjoint.

Palette indices are fixed: `0=transparent`, `1=rail`, `13=online blue`, `14=battery green`, `15=white`; unused entries equal rail. Preserve source RGB tokens in tests/docs, but pack nearest DS RGB555. Expected visible hardware colors are rail `(8,9,10)` / `0x2928` / `#424a52`, green `(13,27,10)` / `0x2b6d` / `#6bde52`, blue `(12,23,30)` / `0x7aec` / `#63bdf7`, and white `0x7fff` / `#ffffff`.

`statusBattery.png` becomes eleven 16x8 frames: DSi 0..4, charging 5..9, NTR outline 10. White is outline/charging mark; green is only DSi fill. `statusProfile.png` is the dedicated 16x16 asset. The runtime block remains 640 bytes: rail 256 plus three 128-byte slots. Maximum status use is 16 OAM entries, 2,176 OBJ-VRAM bytes including four labels, and four simultaneous palette rows; expected top-view usage is at most about 44/128 OAM.

## File Changes

| File | Action | Description |
|---|---|---|
| `arm9/source/romBrowser/views/StatusBarFormat.h` | Modify | Fixed palette, quantization, fallback text, exact bounds/budgets |
| `arm9/source/romBrowser/views/RomBrowserTopScreenView.{h,cpp}` | Modify | Fixed colors, border removal, profile, truthful fallback |
| `arm9/gfx/status{Speaker,Battery}.{png,grit}` | Modify | White speaker and truthful battery frames |
| `arm9/gfx/statusProfile.{png,grit}` | Create | Compact profile placeholder |
| `tests/status_bar_format_tests.cpp`, `tests/test_status_bar_assets.py` | Modify | RED contracts and asset/palette assertions |

## Testing and Physical Acceptance

Strict TDD starts with RED assertions for exact tokens/RGB555 values, index 1 at every rail row, all frames, NTR threshold, NTR volume absence, fallback strings/no-write mode, exact disjoint bounds, assets, and revised budgets. Then implement GREEN and refactor only duplication.

| DSi XL matrix | Required evidence |
|---|---|
| Light, dark, custom themes | Rail is uniform through `y=15`; no border/bracket; `y>=16` unchanged |
| DSi segments 0..4 x charging off/on | Ten states: white outline, exact green fill, independent charge mark, no percentage |
| NTR levels 3 and 4 | `LOW` then `HIGH`, white outline/text, no volume |
| DSi volume 0, 31, invalid | White `00`, white `31`, then icon/number absent |
| Short, 10-glyph, overwide, unsupported names | Blue text, stable ellipsis/`?`, visible avatar/right edge, no collision |
| Each row during refresh | Center remains centered; no overlap or flicker; capture photo/result |

## Threat Matrix

N/A — no routing, shell, subprocess, VCS/PR automation, executable classification, or process-integration boundary.

## Migration / Rollout and Rollback

No migration or flag. Release only after the full matrix passes. Named rollback **Plain-text status rollback** reverts only the view/helper/status assets/tests to the validated plain-text presentation; acquisition/IPC files and unrelated work remain untouched.

## Open Questions

None.
