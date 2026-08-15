# Design: Compose Status Text on Theme Backgrounds

## Technical Approach

`App` injects the active background into `RomBrowserTopScreenView`. The view owns status values and fallback; backgrounds own preparation/publication. A narrow NFT2 renderer produces bounded 4-bit masks. Custom composes RGB555; material retains tiled BG0 and supplies an exact uniform endpoint for antialiased OBJ palettes.

## Architecture Decisions

| Decision | Alternatives / tradeoff | Choice and rationale |
|---|---|---|
| Ownership | Raw labels/addresses couple lifetimes. | Add value/span request-result contracts to `IThemeBackground`; no VRAM address crosses the boundary. |
| RGB555 compositor | Incremental blending ghosts. | Restore 256x16 from pristine, clip half-open bounds, and blend channels as `(dst*(15-a)+fg*a+7)/15`, preserving bit 15. |
| NFT2 coverage | Generic changes risk game labels. | A narrow renderer reuses NFT2 ellipsis APIs and emits unquantized nibbles; quantization remains fallback-only. |
| Custom | VRAM reads or 96 KiB retention waste RAM. | Validate/upload 98,304-byte `topbg.bin`, retain 8 KiB pristine plus 8 KiB composed, release the load buffer, and upload dirty strips in VBlank. Reload/destruction releases strips. |
| Material | Conversion/recomposition is larger. | Keep tiled 8bpp BG0 and 4bpp OBJ labels. Guard that y=0..15 resolves to BG palette index 16; after palette finalization, `LoadResources` copies that exact RGB555 endpoint. Failure is unsupported; never sample or convert the full material. |
| Material OBJ palettes | One row cannot encode both foregrounds. | The view owns `std::array<uint16_t,32>` (64 bytes): `[0,16)` exact endpoint→`StatusText` for date/time/NTR; `[16,32)` endpoint→`NicknameText`. Entries map coverage 0..15 and entry 15 equals its foreground. `Draw` gives each half to `DirectPalette`; normal labels share one allocated row, nickname uses the other. Existing `PaletteManager` uploads rows in VBlank. Rebuild on supported endpoint connection; stop using them on reload, reconstruction, or fallback. |
| Failure | Hidden labels can lose state. | Custom hides text OBJ; material draws full coverage. Unsupported/allocation failure restores pristine and draws truthful binary date/time, NTR, nickname, and DSi battery fallback atomically. |
| Covers/timing | Whole-strip DMA created VBlank starvation. | Preserve BG3/WIN0 precedence and background-before-view VBlank order; publish bounded dirty rows in VBlank and flush each ARM9 DMA source row; no whole-strip atomic publication remains. |
| Readability geometry | Centered label boxes do not center actual glyph ink. | Select Medium9. Final accepted geometry uses `StatusNicknameRight=256` and `StatusBatteryGap=18`, retains battery coordinates from the accepted 4px-left candidate, and applies DSi/NTR clamps to prevent overlap. |

## Data Flow

`RefreshStatus -> NFT2 masks -> background Prepare -> {custom dirty BG | material endpoint OBJ | fallback OBJ} -> App VBlank background -> view/cover`

## File Changes

| File | Action | Description |
|---|---|---|
| `arm9/source/gui/views/Nft2CoverageRenderer.{h,cpp}` | Create | Bounded full-coverage raster primitive. |
| `arm9/source/themes/background/StatusStripComposition.h` | Create | Pure compositor, request/result contracts. |
| `arm9/source/themes/background/IThemeBackground.h` | Modify | Optional prepare/restore capability. |
| `arm9/source/themes/custom/CustomSubBackground.{h,cpp}` | Modify | Validated strips, lifecycle, VBlank DMA. |
| `arm9/source/themes/material/MaterialSubBackground.{h,cpp}` | Modify | Uniform guard and exact BG-palette endpoint lifecycle. |
| `arm9/source/App.cpp` | Modify | Inject background at all three view reconstruction sites. |
| `arm9/source/romBrowser/views/RomBrowserTopScreenView.{h,cpp}` | Modify | Masks, mode selection, two-row material palette bank, truthful fallback. |
| `tests/status_strip_compositor_tests.cpp` | Create | Pure RED-first contracts. |
| `tests/status_bar_format_tests.cpp` | Modify | Geometry, fallback, material/source contracts. |

## Interfaces / Contracts

Request spans are synchronous; results contain no pointers. Custom adds 16 KiB RAM (two 8 KiB strips); BG VRAM remains 96 KiB. Material adds 64 bytes (two 16-entry RGB555 gradients) and no BG VRAM; existing palette-manager upload storage is not duplicated. Custom removes three text OBJ draws; material/fallback retain OAM and the 640-byte speaker/battery allocation. Reconstruction restores pristine.

## Testing Strategy

Strict TDD starts with failing host tests for coverage 0/1/8/15 rounding, bit 15, clipping/canaries, measured Medium9 baseline/ink bounds, worst strings, nickname `[190,256)`, restoration, bounded dirty-row publication with ARM9 DMA source-cache flush, failures, mode truth tables, material uniformity/exact endpoint, and both 16-entry palette rows (layout, distinct foreground endpoints, and 64-byte size). Then run focused C++, Python, and BlocksDS checks.

Physical DSi acceptance is intentionally limited by maintainer decision to the final observed configuration with covers active. The original 5×4 Cartesian matrix remains **NOT EXECUTED** and **NOT PASSED**; no theme, mode, minute, shortening, NTR, or battery-state coverage beyond automated contracts and that observed configuration may be inferred. Archive evidence must label this an intentional limited-scope close.

## Threat Matrix

N/A — no routing, shell, subprocess, VCS/PR automation, executable classification, or process-integration boundary.

## Migration / Rollout

Preserve the uncommitted accepted rail/profile removal, colors, nickname bounds, compact graphics, and quantizer tests. Move threshold rendering behind fallback rather than deleting it. Do not touch `.atl`, archives, current capability specs, assets, or unrelated edits. Roll back by forcing `Unsupported` and restoring pristine.

Work units for auto-chain: (1) compositor/renderer RED-GREEN tests; (2) background contracts/backends; (3) App/view selection plus build/hardware evidence. Each is independently testable and reversible if the forecast exceeds 800 changed lines.

## Open Questions

None.
