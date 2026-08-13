## Exploration: Style the DSteam status bar

### Current State
`RomBrowserTopScreenView` renders the validated status values as three `Label2DView` children on the 256x192 top screen: volume at `(4,0)` in a 44x16 logical region, date/time plus battery at `(48,0)` in a centered 140x16 region, and an ellipsized nickname at `(188,0)` in a right-aligned 64x16 region. The labels use `Medium7_5`, `surfaceBright`, and `onSurface`; the status refresh remains every 60 frames. `StatusBarFormat.h` owns host-tested formatting, including clamped DSi volume, four truthful battery segments, charging, and DS/NTR HIGH/LOW fallback semantics.

`Label2DView` rounds each width to 32px and renders 32x16 4bpp OBJ cells. The current status therefore costs 9 OAM entries and 2,304 bytes of sub OBJ VRAM: 2/512 for volume, 5/1,280 for center status, and 2/512 for nickname. Color index zero remains transparent, so setting a label background color does not produce a contiguous opaque rail. The sub engine has 128 OAM entries and 16KB OBJ VRAM; a typical material top view is approximately 37 OAM entries and 11-12KB before the proposed decoration. Allocation classes do not enforce bounds, so implementation should retain explicit accounting.

Material and custom themes use different top-background engines (`BG0` tiled 8bpp versus `BG2` 16bpp bitmap), while both expose the generated `MaterialColorScheme`. A background-layer rail would therefore need two implementations and could overwrite custom artwork. The top-screen cover also uses `BG3` and `WIN0`, which makes window/blending tricks unnecessarily coupled. An OBJ rail can cover `x=0..255, y=0..15` consistently without changing either background pipeline.

The asset build already auto-discovers `arm9/gfx/*.png` and requires a neighboring `.grit` file. Existing recolorable icons use tiled 4bpp data with `-gt`, `-gB4`, and `-p!`; generated headers are included by basename. Tiny speaker and battery-frame assets can use that path without Makefile changes.

### Affected Areas
- `arm9/source/romBrowser/views/RomBrowserTopScreenView.cpp` — owns status labels, layout, theme colors, VRAM initialization, and the appropriate place to draw a rail and icons without touching acquisition or IPC.
- `arm9/source/romBrowser/views/RomBrowserTopScreenView.h` — would hold visual state plus small rail/icon VRAM offsets and a `Draw` override.
- `arm9/source/romBrowser/views/StatusBarFormat.h` — should preserve existing semantics while exposing pure date/time, volume-visibility, battery-mode, segment, and charging visual state.
- `tests/status_bar_format_tests.cpp` — can prove visual-state mapping and layout bounds on the host without emulating DS graphics.
- `arm9/gfx/` — recommended location for one recolorable speaker icon and a compact battery frame sheet, each paired with the existing 4bpp tiled `.grit` format.
- `arm9/source/gui/views/Label2DView.cpp` — constrains cell rounding, palette use, OAM order, and transparency; no generic change is recommended.
- `arm9/source/themes/material/MaterialColorSchemeFactory.cpp` and custom theme metadata — provide palette inputs and reveal collision risk; no theme schema change is recommended.

### Approaches
1. **Shared OBJ rail plus existing labels and tiny GRIT icons** — draw eight adjacent 32x16 sprites from one opaque tile, then draw resized/repositioned labels and 8x8/16x8 icons above them.
   - Pros: Smallest localized route; one rendering path across material and custom backgrounds; no new background/window system; existing font, palette, VRAM, and asset pipelines remain in use; dynamic battery frames remain truthful.
   - Cons: Adds 10 OAM entries and about 352 bytes before label resizing (256 rail + 32 speaker + 64 battery); permanently reserves the top 16px over custom artwork; same-priority OAM ordering must keep the rail before labels/icons.
   - Effort: Medium

2. **Single composed 256x16 status canvas** — replace the three labels with a dedicated view that renders text and procedural icons into one 4bpp OBJ buffer.
   - Pros: Exact pixel control; one palette row; 8 OAM entries and 2,048 bytes total, slightly less than the current status; no icon assets.
   - Cons: Duplicates `Label2DView` text/tile/VBlank behavior, adds a specialized renderer, increases glyph and tile-layout test burden, and creates more infrastructure than this visual pass needs.
   - Effort: Medium-High

3. **Bake or paint the rail into theme backgrounds** — alter the material tile map and custom bitmap top rows, leaving only text/icons as OBJ.
   - Pros: Minimal OAM cost and a mathematically contiguous rail.
   - Cons: Requires separate BG0/BG2 code paths, mutates custom theme artwork, complicates theme previews/resources, and still needs dynamic OBJ battery/volume visuals. It violates the smallest-route constraint.
   - Effort: High

### Recommendation
Use approach 1. Keep the change inside `RomBrowserTopScreenView`, `StatusBarFormat.h`, focused host tests, and two tiny recolorable assets. Generate the opaque rail tile at VRAM initialization rather than adding a rail image. Draw the rail after the theme/file-info content but before status labels and icons; because `OamManager` allocates downward, later label/icon allocations receive lower OAM indices and remain visually above the rail. Resize labels after removing debug words (`VOL`, `BAT`, brackets) so the added 352-byte graphics cost is offset and the total sub OBJ VRAM remains at or below the current status allocation where practical.

Compact visual direction and tokens:
- **Geometry:** 16px-high rail at `y=0..15`, 4px outer padding, 2px icon/text gaps, and a 1px bottom keyline. Keep the existing `Medium7_5` font.
- **Rail (`#111820` reference):** select the darkest neutral candidate from the active scheme rather than hard-coding the reference.
- **Text (`#F2F5F7` reference):** select the lightest surface/on-surface candidate for contrast.
- **Muted (`#A7B2BC` reference):** mix foreground toward the rail for date separators and secondary marks.
- **Accent (`#66C0F4` reference):** use the theme primary only when contrast is adequate; otherwise use foreground. Apply it to active battery segments and the profile keyline, not the whole rail.
- **Edge (`#2A3947` reference):** low-contrast foreground/rail mix for the bottom keyline.
- **Signature:** a short theme-accent profile keyline before the right-aligned nickname; this creates profile hierarchy without a capsule or desktop-sized control.

Actual 256px layout target:

```text
x=0                                                                    x=255
┌────────────────────────────────────────────────────────────────────────┐ y=0
│  <)) 11       03/17 11:34      [||||]+     │  nickname…              │ y=4..11
└────────────────────────────────────────────────────────────────────────┘ y=15
   4   12 28     60         126      136 156    178 184             252
```

The speaker and number disappear together when volume is unavailable. DSi battery levels map to 0-4 filled segments using the existing `BatterySegments` rule; charging adds a small external mark without changing the segment count. DS/NTR shows a neutral battery outline plus explicit `HIGH` or `LOW`, never synthetic segments or a percentage. Nickname remains ellipsized and right-aligned.

Host tests should cover: volume icon/text visibility and 0-31 clamp; DSi segment boundaries `0`, `1-4`, `5-8`, `9-12`, `13-15`; DS/NTR HIGH/LOW threshold; charging decoration independence; representative light/dark theme token selection; and compile-time/runtime assertions that all fixed groups remain ordered within `0..255`. Pixel appearance still needs emulator or hardware visual confirmation in a later verification phase.

### Risks
- The top 16px becomes an explicit reserved status region; custom themes that intentionally place artwork or configured text there will be covered. Adding per-theme opt-outs now would expand scope and is not recommended.
- `AscendingStackVramManager` and `OamManager` do not bounds-check. The recommended delta is small, but implementation should account for sub OBJ usage and keep the rail sprites sharing one tile.
- `SimplePaletteManager` has only 16 rows and does not deduplicate identical requests. Rail and icon drawing should allocate one shared palette row per frame, while labels retain their existing rows.
- GRIT frame ordering and transparent index zero must be verified for the battery sheet; otherwise state-to-frame offsets can render the wrong segment count.
- CodeGraph did not surface the uncommitted status implementation, so targeted filesystem reads were required after the mandated CodeGraph-first pass.

### Ready for Proposal
Yes. The proposal should lock the change to a 16px OBJ overlay, existing status semantics and polling, two tiny 4bpp recolorable icon assets, pure visual-state tests, and explicit custom-theme/top-screen resource checks. It should reject BG-pipeline changes, quick settings, IPC/status changes, percentages, and per-theme configuration for this pass.
