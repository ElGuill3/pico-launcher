# DSteam Status Bar Presentation Specification

## Purpose

Define the fixed branded DSteam status presentation on the ROM-browser top screen.

## Requirements

### Requirement: Fixed Opaque Rail

The system MUST render one opaque 256x16 RGB `#3f4854` rail at y=0..15. It MUST NOT render a bottom edge, keyline, border, or separator, and MUST preserve all theme content at y>=16.

#### Scenario: Exact rail geometry and palette

- GIVEN any active theme
- WHEN the status rail is rendered
- THEN every rail pixel is opaque `#3f4854` within [0,256)x[0,16)
- AND no rail decoration is rendered at y=15 or below

### Requirement: Truthful White Status Indicators

The system MUST render the speaker icon and date/time in white. It MUST render a white battery outline with filled DSi battery state exactly `#6fdc50`. DSi state MUST show 0..4 segments and charging independently; NTR state MUST show `HIGH` above its existing threshold or `LOW` otherwise. The system MUST NOT show a battery percentage.

#### Scenario: DSi battery and charging state

- GIVEN each DSi segment level with charging enabled or disabled
- WHEN the center group is rendered
- THEN it shows the matching green fill, white outline, and independent charging state

#### Scenario: NTR battery state

- GIVEN NTR battery input above or at/below the low threshold
- WHEN the center group is rendered
- THEN it shows the white outline with `HIGH` or `LOW` and no percentage

### Requirement: Valid Volume Presentation

The system MUST show a white speaker with one of four dynamic volume frames if and only if DSi mode and `VOLUME_VALID` are true. It MUST clamp valid volume input to 0..31 before selecting the frame and MUST hide the speaker when volume is invalid, unavailable, or in NTR mode. The final presentation MUST NOT render a numeric volume value.

#### Scenario: Volume bounds and absence

- GIVEN valid out-of-range volume or invalid volume
- WHEN the left group is mapped
- THEN valid volume is clamped to 0..31 and mapped to the corresponding dynamic speaker frame
- AND invalid volume renders no speaker

### Requirement: Profile Treatment and Collision-Free Composition

The profile group MUST contain a fully opaque, square, symmetric 16x16 profile tile at the far right with a lighter background and centered blue/white silhouette. Its nickname MUST be blue to denote online styling and MUST use the final accepted left bound `[186,236)`. The system MUST NOT render a trailing blue strip, former bracket, or keyline appearance. Unsupported nickname glyphs MUST use a visible fallback glyph, and long names MUST ellipsize. The final accepted battery bounds MUST be `[170,186)` and the profile tile bounds MUST be `[240,256)`; all left, center, and right half-open bounds MUST remain disjoint under maximum representative values.

#### Scenario: Maximum profile layout

- GIVEN maximum volume, date/time, battery, and nickname values
- WHEN layout is calculated
- THEN all three group bounds are disjoint
- AND the blue nickname and complete square profile tile remain visible without a trailing strip

#### Scenario: Nickname fallback

- GIVEN an unsupported glyph or overlong nickname
- WHEN the profile group is mapped
- THEN a visible fallback glyph or ellipsis is used without moving neighboring bounds

### Requirement: Graphics-Allocation Truthful Fallback

The system MUST expose host-testable palette, geometry, state, and bounds mapping. The final status graphics allocation is 1,024 bytes: a 256-byte rail, four 128-byte speaker frames, a 128-byte battery slot, and a 128-byte profile slot. Runtime status presentation MUST remain within 16 status OAM entries, 2,560 OBJ-VRAM bytes, and four palette rows. If status-graphics allocation fails, it MUST NOT silently omit battery semantics or write outside graphics bounds; it MUST present a truthful text-only battery state: DSi segments plus charging state, or NTR `HIGH`/`LOW`, without a percentage.

#### Scenario: Allocation failure retains battery semantics

- GIVEN status graphics cannot be allocated
- WHEN status presentation refreshes
- THEN no graphics write occurs outside the valid allocation
- AND the applicable truthful battery state remains visible as text

### Requirement: Preserved Pipeline and Release Evidence

The final delivery MUST include the prerequisite status exposure over RTC IPC (`bb90392`) and the presentation implementation (`cf1feea`). The status-bar presentation MUST preserve the established polling, acquisition, fallback, browser behavior, and lower-screen content semantics; this capability does not claim that IPC is unchanged. Release evidence MUST include host tests, the BlocksDS build, exact candidate source/object attribution, and iterative physical DSi acceptance for the final presentation. The evidence MUST distinguish that acceptance from the original exhaustive physical matrix, whose individual cells were not separately recorded.

#### Scenario: Iterative physical acceptance

- GIVEN the final launcher on a DSi with the accepted status-bar implementation
- WHEN the final presentation is reviewed on hardware
- THEN the horizontal battery, dynamic speaker, date/time, square symmetric lighter profile tile, and nickname placement are accepted without a corrective reversal
- AND the release record explicitly identifies the result as iterative product-level acceptance rather than an exhaustive matrix

## Release Evidence Caveat

The accepted ROM is 520704 bytes with SHA-256 `57089d364bc6fc2bb83a49596a6ac7961958bd93dfeb422b9332212ed1930c6c`. Exact rebuild-byte reproducibility is not guaranteed because libtwl archive-member ordering can make fresh ROM hashes differ even when source files and compiled objects are byte-identical; source/object attribution remains the release identity evidence.
