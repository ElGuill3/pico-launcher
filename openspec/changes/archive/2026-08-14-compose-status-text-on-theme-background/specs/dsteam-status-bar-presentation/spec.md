# Delta for dsteam-status-bar-presentation

## MODIFIED Requirements

### Requirement: Fixed Opaque Rail

The system MUST NOT render a fixed rail, profile placeholder, separator, or sampled-color overlay. The active theme MUST own the top background; pixels outside glyph coverage MUST remain bit-exact.
(Previously: y=0..15 was replaced by an opaque fixed RGB rail.)

#### Scenario: Theme pixels survive composition
- GIVEN a custom theme with arbitrary RGB555 top-strip pixels
- WHEN status text is composed
- THEN pixels outside glyph coverage are unchanged and no rail or profile placeholder appears

#### Scenario: Unsupported representation
- GIVEN the background cannot satisfy the composition contract
- WHEN status presentation refreshes
- THEN the theme is not replaced by a guessed backing color and truthful binary OBJ text is used

### Requirement: Profile Treatment and Collision-Free Composition

The system MUST compose date/time, NTR `HIGH`/`LOW`, and nickname with full NFT2 coverage on supported backgrounds. Nickname coverage MUST remain exactly `[190,256)`, including existing unsupported-glyph fallback and ellipsis semantics. Speaker and battery bounds and semantics MUST remain unchanged; no profile tile or trailing strip is rendered. Date/time, NTR fallback, and nickname MUST use the selected font's measured baseline in y=0..15.
(Previously: A square profile tile and blue nickname were rendered over a fixed rail.)

#### Scenario: Coverage and nickname bounds
- GIVEN supported date/time, NTR state, and nickname text
- WHEN the strip is composed
- THEN full glyph coverage is applied and nickname writes remain within `[190,256)`

#### Scenario: Nickname shortening
- GIVEN an unsupported glyph or overlong nickname
- WHEN the nickname is mapped
- THEN the visible fallback or ellipsis remains without moving neighboring bounds

#### Scenario: Readable centered status text
- GIVEN the selected status font and the 16-pixel strip
- WHEN date/time, NTR fallback, or nickname is rendered
- THEN its baseline is derived from that font's ascend/descend metrics and its ink remains within y=0..15

### Requirement: Graphics-Allocation Truthful Fallback

The system MUST expose host-testable composition, palette, geometry, state, and bounds contracts. Each recompose MUST restore pristine pixels first, MUST write only y=0..15 and covered strip bounds, and MUST NOT accumulate or ghost text. Custom direct-RGB555 backgrounds MUST be supported. Material MAY be supported only under an executable invariant proving a uniform top-strip RGB555 color, which MUST be the exact composition endpoint. Animated backgrounds and arbitrary material top artwork remain unsupported until they provide this contract. On unsupported representation/invariant, allocation failure, or unavailable composition, the system MUST restore pristine pixels and use truthful binary OBJ text without stale text or lost DSi charging/segment or NTR `HIGH`/`LOW` semantics.
(Previously: Fixed rail/profile allocation was the composition backing and fallback covered allocation failure only.)

#### Scenario: Text changes recompose cleanly
- GIVEN a pristine strip with previously composed text
- WHEN the minute, nickname, or NTR state changes
- THEN pristine pixels are restored before composition and no old glyph remains

#### Scenario: Material guard and failure
- GIVEN uniform material BG0 or an unsupported representation
- WHEN composition is attempted
- THEN uniform material uses its exact endpoint, while unsupported input selects truthful binary OBJ fallback

### Requirement: Preserved Pipeline and Release Evidence

Speaker/battery rendering, status acquisition, polling, fallback, browser behavior, and lower-screen semantics MUST remain unchanged. BG3/WIN0 cover precedence MUST remain intact. Composition publication MUST occur only during VBlank-safe processing. Theme load, mode changes, and view reconstruction MUST restore or re-establish pristine/composition state. Timing observed physically MUST keep covers active. Release evidence MUST state whether acceptance is exhaustive or intentionally limited.
(Previously: lifecycle and timing were not composition contracts.)

#### Scenario: Host and limited physical acceptance
- GIVEN host contracts, a built launcher, and maintainer-selected limited physical scope
- WHEN evidence is recorded for the final observed configuration
- THEN clipping, restoration, fallback, lifecycle, invariant, and build evidence are proven
- AND the final observed layout is accepted only with covers active
- AND the original 5×4 matrix remains **NOT EXECUTED** and **NOT PASSED**, with archive evidence declaring the intentional limited-scope close

## REMOVED Requirements

### Requirement: Fixed Rail and Profile Placeholder

(Reason: Theme-owned composition replaces opaque rail and profile placeholder behavior.)
(Migration: Use the modified composition and nickname requirements.)
