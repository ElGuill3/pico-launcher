## Exploration: Compose status text on theme-owned top backgrounds

### Current State
The uncommitted baseline removes the fixed rail and profile tile, keeps the nickname at `[186,236)`, and renders date/time, NTR `HIGH`/`LOW`, and nickname through three 4bpp `Label2DView` OBJ labels. Those labels use NFT2 coverage, but status rendering disables antialiasing and quantizes each nibble at threshold 8. This avoids a fixed-color antialiasing halo because OBJ palette index 0 is the only transparent value; every intermediate coverage index is otherwise opaque. Hardware evidence shows the tradeoff is still unacceptable: threshold 8 clips/aliases edge pixels and threshold 12 fragments glyphs.

Game metadata does not provide a reusable transparent-OBJ solution. It uses the same `Label2DView`/NFT2 path, but remains stable because its gradient palette blends coverage between a known opaque card or configured custom-theme `blendColor` and the foreground. The reusable part is NFT2's full 0..15 coverage and color interpolation, not the game label's final OBJ representation.

The top backgrounds have different representations and are loaded before the top-screen view is created. Custom themes upload a static 256x192 direct RGB555 bitmap to sub BG2 at `BG_GFX_SUB + 0x8000`; the temporary 96 KiB load buffer is then released. Material uploads a static tiled 8bpp BG0 plus map and creates a gradient palette; its current y=0..15 source pixels are all palette index 16. BG3 plus WIN0 is independently used for covers, normally beginning at y=18 but custom positioning can intersect the top strip. Background VBlank runs before `RomBrowserTopScreenView::VBlank`.

Speaker and battery are separate OBJ sprites and can remain unchanged. If their shared status allocation fails, the current code draws date/time, truthful battery text, and nickname through labels; that truthful fallback must survive even if its typography is degraded.

### Affected Areas
- `arm9/source/themes/background/IThemeBackground.h` — natural ownership boundary for exposing an optional status-text composition surface and its lifecycle.
- `arm9/source/themes/custom/CustomSubBackground.{h,cpp}` — can retain a pristine 256x16 RGB555 strip, rebuild composed pixels, and upload only that strip.
- `arm9/source/themes/material/MaterialSubBackground.{h,cpp}` — knows the actual top color and can support the current uniform strip without converting the full 256x192 background.
- `arm9/source/App.cpp` — currently owns both the top background and top view, and is the smallest place to connect them without global state.
- `arm9/source/romBrowser/views/RomBrowserTopScreenView.{h,cpp}` — owns text values, refresh cadence, OBJ fallback, status allocation, and VBlank ordering.
- `arm9/source/gui/views/LabelView.cpp` / a small host-testable compositor helper — NFT2 rasterization behavior should be reused or narrowly factored; generic `Label2DView` OBJ behavior should not be rewritten.
- `tests/status_bar_format_tests.cpp` and focused compositor tests — need blend, clipping, geometry, lifecycle, and failure contracts without DS hardware.

### Approaches
1. **Representation-aware background composition (recommended)** — add a narrow optional background surface. Custom retains and recomposes an 8 KiB pristine RGB555 strip; material keeps BG0 and uses its known opaque top color with full NFT2 palette antialiasing, or equivalently patches only its uniform top tiles.
   - Pros: Preserves theme ownership and custom pixels; avoids a full material conversion; reuses NFT2 coverage; updates only when strings/background change; no extra OBJ text in the normal custom path.
   - Cons: Two small backend strategies; material support depends on the verified invariant that its current top strip is one opaque color; requires an explicit App-to-view composition seam and careful fallback.
   - Effort: Medium

2. **Standardize both backgrounds on direct RGB555 and compose one common strip** — convert material BG0 into a BG2-style 16bpp bitmap, then use one RGB555 compositor for both themes.
   - Pros: One pixel model; arbitrary future material artwork works; simple exact blending against every destination pixel.
   - Cons: Converts the whole material background to solve 4,096 pixels; expands BG VRAM from compact tiles/map to about 96 KiB; changes display mode/resource setup and increases cover/window regression risk. It is not the smallest robust first slice.
   - Effort: High

3. **Keep status text as OBJ with a sampled or fixed blend color** — enable NFT2 antialiasing and choose one background color per label/theme.
   - Pros: Very small change; directly mirrors stable game metadata rendering; no BG lifecycle work.
   - Cons: Only correct over a uniform known background. Arbitrary custom pixels produce visible color halos, so this does not satisfy the product intent.
   - Effort: Low

4. **Dedicated direct-color overlay BG for the 16px strip** — render composed pixels into another BG layer while leaving both existing backgrounds intact.
   - Pros: One compositor and no mutation of pristine theme VRAM.
   - Cons: Consumes scarce sub-engine BG/VRAM state, needs transparent direct-color semantics and priority/window coordination, and couples directly to BG3 cover/WIN0 behavior. It is larger and riskier than patching the background owner.
   - Effort: High

### Recommendation
Use approach 1 and keep conversion of material BG0 out of the first slice. Define a narrow composition contract owned by the active top background, connected by `App` to `RomBrowserTopScreenView`. Feed it the already-formatted date/time, optional NTR text, nickname, exact fixed bounds, foreground colors, font, and full NFT2 coverage. The custom backend should retain a pristine 8 KiB strip and a composed 8 KiB strip (about 16 KiB persistent RAM), rebuild from pristine whenever visible text changes, and copy 8 KiB during VBlank after the background has configured BG2. Do not blend incrementally into previously composed pixels.

For material, preserve tiled BG0. Its checked-in top 16 rows are uniformly palette index 16, so the materially smaller robust implementation is to use that background's exact generated RGB value as the label blend endpoint and keep full NFT2 antialiasing. Guard this invariant with a test/source contract; if the material top ceases to be uniform, that backend must either decode/compose an exact strip or adopt direct RGB555 later rather than silently sampling one color.

Normal composition should remove the three status text OBJ labels from the successful custom path, while speaker and battery stay exactly as current OBJ sprites/colors. Keep the existing binary OBJ labels as an explicit last-resort fallback when the composition surface is unavailable, allocation fails, or its invariant is unsupported. The fallback must remain truthful and bounded, but it is allowed to be visually degraded; failure must never leave stale composed text, erase pristine theme pixels, or hide NTR/battery semantics.

First slice scope: date/time, NTR fallback text, and nickname only; fixed nickname bounds `[186,236)`; custom direct-RGB555 and current material themes; recomposition on initial background/view connection and actual text changes; clean restoration before every rebuild; focused host tests, container build, and physical DSi evidence. Non-goals: speaker/battery redesign, profile restoration, fixed rail, IPC/polling/format changes, new theme metadata, animated backgrounds, arbitrary future material top art, game-card renderer redesign, and historical artifact edits.

Acceptance evidence should include: host-tested RGB555 interpolation at coverage 0/1/8/15; exact clipping to each half-open text bound and y=0..15; pristine-strip restoration across shorter text/minute/NTR transitions; nickname fallback/ellipsis at `[186,236)`; material uniform-top invariant; compositor unavailable/allocation-failure fallback; no writes outside 8 KiB; focused C++ tests; Python tests that do not require Pillow (Pillow-dependent checks may skip); BlocksDS v1.16.0 container build; and physical photos on at least dark, light, high-contrast, and textured custom backgrounds plus material, including a live minute change and DSi/NTR fallback states.

### Risks
- Passing raw label objects or BG addresses across owners would create lifetime coupling; the contract should exchange immutable text/style inputs and an explicit availability/result state.
- An 8 KiB VBlank copy is feasible but must be measured on hardware alongside cover upload; dirty-row/rectangle DMA can be a later optimization only if full-strip copying causes missed frames.
- Custom cover positions may overlap y=0..15; composition changes BG2 only, while BG3/WIN0 can still visually cover it. Acceptance must define cover precedence rather than accidentally rewriting cover behavior.
- Material's uniform-top optimization is correct today but not future-proof unless guarded and failed closed.
- `Label2DView` allocation does not validate failed offsets generally; the retained OBJ fallback must not introduce new unchecked writes.
- The archived specification still requires the removed rail/profile and must be changed only through a new delta spec in a later phase, never edited in place.

### Ready for Proposal
Yes. The proposal should select representation-aware composition, explicitly reject full material conversion for the first slice, retain binary OBJ text only as a truthful degraded fallback, and require hardware evidence over adversarial theme pixels. Delivery should be planned as reviewable slices under the 800-line budget: pure compositor/tests first, background integration second, then status-view fallback and hardware acceptance.
