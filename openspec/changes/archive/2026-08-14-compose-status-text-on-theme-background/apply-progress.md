# Apply Progress: Compose Status Text on Theme Backgrounds

## Work Unit
- Units 1–3 are cumulatively complete; limited physical acceptance reconciliation closes the PR 3 evidence boundary under auto-chain, stacked-to-main.
- Strict TDD active. Native attempt token: `sha256:235b55b33dc3fdaa0a1b883e25bccf290fa9b3b5468ceafa917a7b512c301e53`; authority is externally acquired (`proceed`) and this work unit did not acquire or settle it.

## Completed Tasks
- [x] 1.1–1.3 compositor/coverage contracts and implementation.
- [x] 2.1–2.3 custom/material backend contracts and implementation.
- [x] 3.1 mode/lifecycle/cover-order/reconstruction/stale-text/DSi fallback RED contracts.
- [x] 3.2 App/view integration: three reconstruction sites inject the top background; custom coverage composition, material direct palettes, atomic modes, and VBlank order.
- [x] 3.3 limited physical acceptance/evidence reconciliation; the exhaustive 5×4 matrix is **NOT EXECUTED** and **NOT PASSED**.

## TDD Cycle Evidence
| Task | RED | GREEN | REFACTOR |
|---|---|---|---|
| 1.1 | Missing composition/coverage interfaces; compile exit 1 | Focused host test exit 0 | Clean |
| 1.2 | Missing StatusStripComposition; compile exit 1 | Focused executable exit 0 | Bounded blend helper |
| 1.3 | Tests preceded production files; compile exit 1 | Both focused executables exit 0 | None needed |
| 2.1 | Missing StatusBackgroundContracts; compile exit 1 | Focused C++ executable exit 0 | Expanded contract cases |
| 2.2 | Contract tests preceded backend changes | Focused C++ host tests exit 0 | Minimal buffers/VBlank path |
| 2.3 | Material tests preceded backend guard | Focused C++ host tests exit 0 | Added packed generated-tile byte-view test before correcting BlocksDS word representation |
| 3.1 | Missing StatusTextMode/StatusTextVisible and source-order assertions; compile exit 1 | Both focused C++ executables exit 0 | Minimal value-mode helpers |
| 3.2 | Integration source assertions failed before wiring | Both focused C++ executables exit 0 | Optional direct palette, reused label renderer |

## Build Remediation
- **Failed evidence remediated**: `sha256:355c8bf0cbb537f896966359ecbb3b490e52a1320527ab6566617c7596caeb05`.
- **Diagnosis**: `statusSpeaker.png` is byte-identical to `HEAD` and to the clean copy (`sha256:56cb0614107487ad49de033123de4514125668963f9422b5c823c2a13cdfb734`); the reported GRIT crash was not reproduced in an isolated serial build. The clean build exposed the actual product defect: `MaterialSubBackground` tried to construct `std::span<const uint8_t>` directly from GRIT's generated `unsigned int[]`. The target toolchain rejects that incompatible element type.
- **Correction**: `MaterialTileBytes` exposes generated 32-bit tile storage as its byte representation; `MaterialSubBackground` passes that bounded byte span to the uniform-strip guard. No asset bytes were modified.
- **Clean-copy runtime harness**: `podman run --rm -v /tmp/pico-launcher-blocksds-remediation:/workspace -w /workspace docker.io/skylyrac/blocksds:slim-v1.16.0 sh -lc 'make clean && make -j1'` followed by `make -j1` after the command timeout; final exit `0`. The serial build logged `GRIT.9 arm9/gfx/statusSpeaker.png` and `NDSTOOL LAUNCHER.nds`.
- **ROM identity**: `/tmp/pico-launcher-blocksds-remediation/LAUNCHER.nds`, 523264 bytes, `sha256:551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f`.
- **Cleanup/process evidence**: the container used `--rm`; no `make`, `grit`, or `podman` child process remained after completion. The isolated copy remains at `/tmp/pico-launcher-blocksds-remediation` only for reproducibility and was not copied back into the worktree.

## Work Unit Evidence
| Evidence | Exact result |
|---|---|
| Focused C++ | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/status-strip-compositor-tests && /tmp/status-strip-compositor-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests` — exit 0; both passed. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4 passed. |
| Runtime harness | Clean BlocksDS v1.16.0 serial build above — exit 0; produced the identified ROM. |
| Asset comparison | Current, clean-copy, and `HEAD` `statusSpeaker.png` hashes all equal `sha256:56cb0614107487ad49de033123de4514125668963f9422b5c823c2a13cdfb734`. |
| Process | `git diff --check` — exit 0. |
| Rollback | Revert `StatusBackgroundContracts.h`, `MaterialSubBackground.cpp`, and the packed-tile test additions in `status_strip_compositor_tests.cpp`; this removes only the BlocksDS type-correct byte-view correction and leaves unrelated integration work intact. |

## Remaining
- Historical limitation: the 20-cell matrix was unobserved at this stage and remains **NOT EXECUTED** and **NOT PASSED** under the final limited-scope close.

## Launcher-Only DSteam Installation Evidence — Centered/Right-Packed Status Layout
- Native token `sha256:63b503564af80b6d4b6b30011599c0db2c8f3567307689fcd3985a14ea733dca` was neither acquired nor settled.
- Independently verified media identity: UUID `0BFC-CD24` resolved to `/dev/sda1`; `findmnt` reported an rw `vfat` mount at `/run/media/guill3/0BFC-CD24`; UDisks independently reported the UUID, `vfat`, filesystem usage, and `ReadOnly=false`.
- Source was verified before and after the transaction without rebuilding: `/tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds`, 542208 bytes, `sha256:c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`.
- Predecessor verification passed: `/_picoboot.nds`, 542208 bytes, `sha256:b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`.
- Created and verified `/_picoboot.nds.backup-pre-horizontal-alignment-20260814T151413Z`: 542208 bytes, `sha256:b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`.
- The candidate was staged in the target directory, fsynced, atomically renamed, and the installed file and directory were fsynced; libc `syncfs` and `sync` completed. Exact target readback matched the candidate.
- Own canonical sorted compact-JSON root manifests: pre `sha256:1bb547ce626eaf22d3e2eec2463c98399c004d6a9e25c678d07b321908e8e6f2`; post `sha256:5f4738e85d6aaa01e7c0fe38b0fec426305edd1e8472ec9731abf39943d940c8`. Semantic diff proved only `/_picoboot.nds` changed and the one verified backup was added; no temporary transaction artifact remained.
- UDisks unmounted `/dev/sda1`; independent `findmnt -S`, `findmnt -T`, and UDisks empty-`MountPoints` checks proved absence. Physical matrix remains pending/unaccepted and task 3.3 remains unchecked. This installation makes no visual or no-tearing claim.

## Work Unit Evidence — Centered/Right-Packed Status Layout Installation
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | Source identity and exact target/predecessor/backup/readback SHA-256 gates passed; source before and after was 542208 bytes, `c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`. |
| Runtime harness command/scenario and exact result | UUID-directed same-directory FAT launcher-only transaction passed: backup, atomic replacement, staged/installed/directory fsync, libc `syncfs`, `sync`, exact readback, canonical semantic root diff, and UDisks unmount/absence proof. |
| Rollback boundary | Restore only `/_picoboot.nds` from `/_picoboot.nds.backup-pre-horizontal-alignment-20260814T151413Z`; all other root entries remain outside the rollback boundary. |

## Result Contract — Centered/Right-Packed Status Layout Installation
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:0db2e5f7b23132b40c006dea808b31b69f7d63672bfbb402b6b54e11115d98d7
settlement_evidence_revision: sha256:03702392734788f6e37ef14a98673c6bd87bb3803c5c30960c321b83a3dda330
change: compose-status-text-on-theme-background
work_unit: status-horizontal-alignment-install-final
result: success
native_attempt_token: sha256:63b503564af80b6d4b6b30011599c0db2c8f3567307689fcd3985a14ea733dca
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
media_mountpoint: /run/media/guill3/0BFC-CD24
target: /_picoboot.nds
predecessor_bytes: 542208
predecessor_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
backup: /_picoboot.nds.backup-pre-horizontal-alignment-20260814T151413Z
backup_bytes: 542208
backup_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
candidate_bytes: 542208
candidate_sha256: c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9
readback_bytes: 542208
readback_sha256: c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9
source_preserved: true
pre_manifest_sha256: 1bb547ce626eaf22d3e2eec2463c98399c004d6a9e25c678d07b321908e8e6f2
post_manifest_sha256: 5f4738e85d6aaa01e7c0fe38b0fec426305edd1e8472ec9731abf39943d940c8
manifest_algorithm: sorted_direct_root_entries_compact_json_regular_file_sha256
root_diff: launcher_replaced_plus_one_verified_backup
temporary_artifact_absent: true
durability: staged_file_fsync_atomic_rename_installed_file_fsync_directory_fsync_libc_syncfs_sync
udisks_unmount: success
unmount_proven: true
physical_matrix: pending_unaccepted
task_3_3_completed: false
no_visual_claim: true
no_tearing_claim: true
```

## Blocked Work Unit — medium9-status-font
- Native attempt token `sha256:d0aa61513a495636c3b86d2125bb707f5aa85e5e4adc37f17bfd0ba547f18498` was neither acquired nor settled.
- Preconditions and the Strict TDD safety net passed: the two focused C++ host executables exited 0 before any edit.
- The repository contains only opaque NFT2 assets and Makefile rules that package an existing `.nft2` with `bin2c`; it contains no NFT2 font-generation source, command, font input, or documented generator invocation. No installed `nft2`, `nft2gen`, `fontforge`, `otf2bdf`, or `bdf2nft2` command is available, and the installed font matcher resolves `Noto Sans`, not Noto Sans JP Medium.
- Therefore a genuine measured NotoSansJP Medium 9 NFT2 asset cannot be produced through the repository's existing mechanism. No production source, asset, task checkbox, ROM, SD media, commit, push, PR, archive, or `.atl` path was changed. Task 3.3 remains unchecked and the physical matrix remains pending.

## Work Unit Evidence — medium9-status-font
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests` — exit 0; both pre-edit safety-net executables passed. |
| Runtime harness command/scenario and exact result | N/A — no genuine Medium 9 NFT2 asset exists to package or run. A BlocksDS build would not validate the requested asset and was intentionally not run. |
| Rollback boundary | This appended blocker entry only; no product behavior changed. |

```yaml
schema: gentle-ai.medium9-status-font-result/v1
evidence_revision: sha256:84412683cda01f50f9aef7996e3e83b3bc7caa7cca8c60ea1d7945e1b8481220
settlement_evidence_revision: sha256:eaaae08fe6d1d97b3a83c9cf7872c7a15d0b09aa3173db7d32acb7011c3843ce
change: compose-status-text-on-theme-background
work_unit: medium9-status-font
result: blocked_missing_font_generation_mechanism
native_attempt_token: sha256:d0aa61513a495636c3b86d2125bb707f5aa85e5e4adc37f17bfd0ba547f18498
native_attempt_acquired: false
native_attempt_settled: false
font_asset: not_created
font_metrics: not_measurable
sample_widths: not_measurable
rom: not_created
physical_matrix: pending_unaccepted
task_3_3_completed: false
```

## Launcher-Only DSteam Installation Attempt — Status Horizontal Alignment
- Native token `sha256:9f365158b6255a9a2eb698d6facd3043be1b01d90024109528fe8cd77085664a` was neither acquired nor settled.
- Candidate source was verified before the media preflight and again after it, without rebuilding: `/tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds`, 542208 bytes, `sha256:c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`.
- Independent identity gates agreed before any mutation: `/dev/disk/by-uuid/0BFC-CD24` resolved to `/dev/sda1`; `findmnt` identified `/dev/sda1` mounted at `/run/media/guill3/0BFC-CD24` as rw `vfat`; udev reported `ID_FS_UUID=0BFC-CD24` and `ID_FS_TYPE=vfat`; UDisks reported `IdUUID=0BFC-CD24`, `IdType=vfat`, `IdUsage=filesystem`, and `ReadOnly=false`.
- Expected predecessor verification passed: `/_picoboot.nds`, 542208 bytes, `sha256:b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`.
- **Fail-closed result: no mutation.** The required exact pre-state root manifest encountered `Input/output error` while reading a root entry. Per the transaction gate, no backup, stage file, target replacement, fsync/sync sequence, root diff, or unmount operation was attempted.
- Physical matrix remains pending/unaccepted and task 3.3 remains unchecked. This attempt makes no visual, no-tearing, or physical-acceptance claim. No source, test, task-checkbox, commit, push, PR, or archive mutation occurred.

## Installation Work Unit Evidence — Status Horizontal Alignment
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `stat -c 'source-preserved path=%n size=%s' /tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds && sha256sum /tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds` — exit 0 before and after the preflight; 542208 bytes and `c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`. The predecessor gate passed at 542208 bytes with `b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`. |
| Runtime harness command/scenario and exact result | UUID-directed launcher-only preflight — blocked before mutation because exact root-manifest capture failed with `Input/output error`. |
| Rollback boundary | N/A: no media mutation occurred; `/_picoboot.nds` and every root entry remain outside this executor's rollback boundary. |

## Result Contract — Status Horizontal Alignment Installation Attempt
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:888d1b92119d94123fe213560f09aa79aa8c69e24852aaf74469742e86a024b1
settlement_evidence_revision: sha256:d38a05fe3cf10e4b85bd8a5a9490b025405bf4466f34717701816f98286688f3
change: compose-status-text-on-theme-background
work_unit: status-horizontal-alignment-install
result: blocked_pre_mutation
native_attempt_token: sha256:9f365158b6255a9a2eb698d6facd3043be1b01d90024109528fe8cd77085664a
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
media_mountpoint: /run/media/guill3/0BFC-CD24
media_filesystem: vfat
media_mount_options: rw,nosuid,nodev,relatime,uid=1000,gid=1000,fmask=0022,dmask=0022,codepage=437,iocharset=ascii,shortname=mixed,showexec,utf8,flush,errors=remount-ro
target: /_picoboot.nds
predecessor_bytes: 542208
predecessor_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
candidate_bytes: 542208
candidate_sha256: c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9
source_preserved: true
root_pre_manifest: unavailable_input_output_error
backup_created: false
target_replaced: false
durability: not_started
readback: not_started
root_diff: not_proven_no_mutation
temporary_artifact_absent: true
unmount_attempted: false
unmount_proven: false
physical_matrix: pending_unaccepted
task_3_3_completed: false
no_visual_claim: true
no_tearing_claim: true
```

## Launcher-Only DSteam Installation Evidence — Medium9 Status Font
- Native token `sha256:74d58e66004bda95dc89c1ffba401f7c9c60d84c5502c010de265a2b0804f27a` was neither acquired nor settled.
- Source was verified before the transaction and after unmount without rebuilding: `/tmp/pico-launcher-medium9-status-font/LAUNCHER.nds`, 542208 bytes, `sha256:b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`.
- UUID-directed mount discovery found exactly one rw `vfat` mount: UUID `0BFC-CD24`, `/dev/sda1`, `/run/media/guill3/0BFC-CD24`. The mounted source was independently corroborated by udev (`ID_FS_UUID=0BFC-CD24`, `ID_FS_TYPE=vfat`) and `/sys/class/block/sda1/ro=0`.
- Predecessor gate passed: `/_picoboot.nds`, 523776 bytes, `sha256:ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89`.
- Verified unique backup: `/_picoboot.nds.backup-pre-medium9-status-20260814T125712Z`, 523776 bytes, `sha256:ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89`.
- The candidate was staged in the target directory, file-fsynced and hash-verified, atomically renamed to `/_picoboot.nds`, and the installed file and directory were fsynced. Python lacked `os.syncfs`; the post-rename handler therefore reported that API absence, then `sync` completed successfully before exact readback. This was not an I/O error.
- Exact installed readback passed: 542208 bytes, `sha256:b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`. The transaction staging path was absent. The initial script's root-diff assertion did not execute after the unavailable `os.syncfs` API; consequently, the exact before/after root-diff comparison is an evidence gap, not a claim. The installer itself only created the verified backup, temporary stage, and replacement target.
- `udisksctl unmount --block-device /dev/sda1` returned `Unmounted /dev/sda1.` Independent UUID-directed `findmnt` and `mountpoint` checks found no remaining mount; `lsblk` showed no mountpoint.
- Physical matrix remains pending/unaccepted; task 3.3 remains unchecked. This installation makes no visual, no-tearing, or physical-acceptance claim. No source, tests, task checkboxes, commits, pushes, PRs, or archives were changed.

## Work Unit Evidence — Medium9 Status Font Installation
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `stat -c 'source-preserved path=%n size=%s' /tmp/pico-launcher-medium9-status-font/LAUNCHER.nds && sha256sum /tmp/pico-launcher-medium9-status-font/LAUNCHER.nds` — exit 0 after unmount; source preserved at 542208 bytes and the requested SHA-256. Predecessor and backup gates each passed at 523776 bytes with `ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89`. |
| Runtime harness command/scenario and exact result | UUID-directed, same-directory FAT launcher-only installer — installed candidate and exact readback passed; staged/installed/directory fsync and `sync` passed; UDisks unmount and independent absence passed. Exact root-diff assertion: not completed because `os.syncfs` is unavailable in this Python runtime after the replacement. |
| Rollback boundary | Restore only `/_picoboot.nds` from `/_picoboot.nds.backup-pre-medium9-status-20260814T125712Z`; this retains all other media content. |

## Result Contract — Medium9 Status Font Installation
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:5b140baf22940addbc5372727d27f2b0a933ed59c42d1d7f632e8090787b2e65
settlement_evidence_revision: sha256:3e726fc78365be42e7d40fcb01073a1b43290885e8c9e14287c250f197e5cf90
change: compose-status-text-on-theme-background
work_unit: medium9-status-font-install
result: partial_evidence_gap_root_diff
native_attempt_token: sha256:74d58e66004bda95dc89c1ffba401f7c9c60d84c5502c010de265a2b0804f27a
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
target: /_picoboot.nds
predecessor_bytes: 523776
predecessor_sha256: ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89
backup: /_picoboot.nds.backup-pre-medium9-status-20260814T125712Z
backup_bytes: 523776
backup_sha256: ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89
candidate_bytes: 542208
candidate_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
readback_bytes: 542208
readback_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
root_diff: not_proven_after_post_rename_api_gap
temporary_artifact_absent: true
durability: staged_file_fsync_atomic_rename_installed_file_fsync_directory_fsync_sync
unmount_proven: true
source_preserved: true
physical_matrix: pending_unaccepted
task_3_3_completed: false
no_tearing_claim: false
```

## Launcher-Only DSteam Installation Attempt — Coverage Overflow Fix
- Native token `sha256:fceb40eb8ffb040c5d470a4522f6e806fe90ab59128ef3f8371556848327b118` was neither acquired nor settled.
- Candidate source gate passed before and after the attempt without rebuilding: `/home/guill3/Documents/Hobbies/dspico/pico-launcher/LAUNCHER.nds`, 523776 bytes, `sha256:ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89`.
- Independent identity signals identified the mounted volume as `/dev/sda1`, UUID `0BFC-CD24`, `vfat`, at `/run/media/guill3/0BFC-CD24`; `/dev/disk/by-uuid/0BFC-CD24` also resolved to `/dev/sda1`, and UDisks reported `IdUUID=0BFC-CD24`, `IdType=vfat`, `IdUsage=filesystem`, and `ReadOnly=false`.
- **Fail-closed result: no mutation.** `findmnt` reported the actual mounted filesystem options as `ro,...,errors=remount-ro`, so the writeability gate failed. Independently, reading `/_picoboot.nds` returned `Input/output error`, so its required predecessor identity (523776 bytes, `sha256:960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`) could not be verified.
- No backup, staging file, target replacement, fsync/sync durability sequence, root-diff proof, or unmount operation was attempted. A root manifest could not be completed because existing root entries also returned `Input/output error`. Source, tests, task checkboxes, commits, pushes, and PRs were not changed.
- Physical matrix remains failed/pending and task 3.3 remains unchecked; this attempt makes no physical acceptance claim.

## Installation Work Unit Evidence — Coverage Overflow Fix
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `stat -c '%s' LAUNCHER.nds && sha256sum LAUNCHER.nds` before and after the media gate — exit 0; 523776 bytes and exact requested candidate SHA-256. |
| Runtime harness command/scenario and exact result | Launcher-only preflight against mounted `/dev/sda1` — failed closed before mutation: mount options were `ro,...,errors=remount-ro`; target SHA-256 read returned `Input/output error`. |
| Rollback boundary | N/A: no media mutation occurred; `/_picoboot.nds` and every root entry remain untouched by this executor. |

```yaml
schema: gentle-ai.installation-result/v1
change: compose-status-text-on-theme-background
result: blocked_pre_mutation
native_attempt_token: sha256:fceb40eb8ffb040c5d470a4522f6e806fe90ab59128ef3f8371556848327b118
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
target: /_picoboot.nds
candidate_bytes: 523776
candidate_sha256: ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89
expected_predecessor_bytes: 523776
expected_predecessor_sha256: 960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d
mount_read_only: true
predecessor_verified: false
predecessor_read_error: Input/output error
mutation_performed: false
backup_created: false
unmount_attempted: false
physical_matrix: failed_pending
task_3_3_completed: false
```

## Launcher-Only DSteam Installation Evidence — Coverage Overflow Fix Retry
- Native attempt token `sha256:5797a115a3e8e979c9d107c27d8860bcbadb91f8c0bb5033a1598a027cc20efd` was neither acquired nor settled.
- Independent pre-mutation media identity gates agreed: `/dev/disk/by-uuid/0BFC-CD24` resolved to `/dev/sda1`; `findmnt` identified a read-write `vfat` mount at `/run/media/guill3/0BFC-CD24`; UDisks reported `IdUUID=0BFC-CD24`, `IdType=vfat`, `IdUsage=filesystem`, and `ReadOnly=false`.
- Source was verified before mutation and again after installation without rebuilding: `/home/guill3/Documents/Hobbies/dspico/pico-launcher/LAUNCHER.nds`, 523776 bytes, `sha256:ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89`.
- Predecessor verification passed: `/_picoboot.nds`, 523776 bytes, `sha256:960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`.
- Verified unique backup: `/_picoboot.nds.backup-pre-coverage-overflow-fix-20260814T042431Z`, 523776 bytes, `sha256:960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`.
- The candidate was staged in the target directory, file-fsynced, atomically renamed to `/_picoboot.nds`, then the installed file and root directory were fsynced. `syncfs` (via libc because this Python runtime lacks `os.syncfs`) and `sync` completed; no second replacement occurred. Exact target readback matched the requested candidate.
- Root proof: the pre-state manifest had 51 entries and post-state had 52; the installer created only the verified backup plus a same-directory stage that was atomically renamed. The post-state contains no transaction temporary artifact, so the transaction boundary is launcher replacement plus the one verified backup.
- UDisks unmounted `/dev/sda1`. Independent absence proof: `findmnt -S /dev/sda1` exit 1, `findmnt -T /run/media/guill3/0BFC-CD24` exit 1, `/proc/mounts` did not contain `/dev/sda1`, and UDisks reported an empty `MountPoints` value.
- Physical matrix remains failed/pending and task 3.3 remains unchecked. This installation makes no hardware, no-tearing, or physical-acceptance claim. No source, test, task-checkbox, commit, push, or PR mutation occurred.

## Installation Work Unit Evidence — Coverage Overflow Fix Retry
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `stat -c '%s' LAUNCHER.nds && sha256sum LAUNCHER.nds` — exit 0 after installation; 523776 bytes and `ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89`. Predecessor and backup gates each passed at 523776 bytes with `960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`. |
| Runtime harness command/scenario and exact result | Same-directory FAT launcher-only install — success; exact identity gates, verified backup, atomic rename, file/directory fsync, libc `syncfs`, `sync`, target readback, root boundary/temporary-artifact proof, and UDisks unmount all passed. |
| Rollback boundary | Atomically restore only `/_picoboot.nds` from `/_picoboot.nds.backup-pre-coverage-overflow-fix-20260814T042431Z`; every other root entry remains outside the rollback boundary. |

## Result Contract — Coverage Overflow Fix Retry
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:4c4d09f3f58109dbb5133bcf64e9502e5ad389d404224d71f4fe2fb9a1234b7f
settlement_evidence_revision: sha256:4c4d09f3f58109dbb5133bcf64e9502e5ad389d404224d71f4fe2fb9a1234b7f
change: compose-status-text-on-theme-background
result: success
native_attempt_token: sha256:5797a115a3e8e979c9d107c27d8860bcbadb91f8c0bb5033a1598a027cc20efd
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
target: /_picoboot.nds
predecessor_bytes: 523776
predecessor_sha256: 960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d
backup: /_picoboot.nds.backup-pre-coverage-overflow-fix-20260814T042431Z
backup_bytes: 523776
backup_sha256: 960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d
candidate_bytes: 523776
candidate_sha256: ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89
readback_bytes: 523776
readback_sha256: ae9945251ae13a43fbdc05b4a4e3cdc90c9a758901c510c7d50c5f835c099d89
root_diff: launcher_replaced_plus_one_verified_backup
temporary_artifact_absent: true
durability: staged_file_fsync_atomic_rename_installed_file_fsync_directory_fsync_syncfs_sync
unmount_proven: true
physical_matrix: failed_pending
task_3_3_completed: false
no_tearing_claim: false
```

## Hardware Defect Remediation — vblank-starvation-remediation
- Native token `sha256:c1e88c80e276e43f9c6b58509aec3819f991d764bb86309e34d139f00c936184` was neither acquired nor settled.
- **Bounded correction**: reverted only a75a's waited whole-strip 8 KiB atomic publication to c118's clipped dirty-row transfer behavior in `CustomSubBackground.{h,cpp}` and removed `AtomicStatusStripPublication` from `StatusBackgroundContracts.h`. RAM composition and dirty-plan merging remain intact.
- **Cache coherency**: `DC_FlushRange` now precedes every ARM9 DMA source-row transfer. BlocksDS/libnds `nds/dma.h` explicitly states DMA cannot access the ARM9 data cache and requires a source flush; `_composedStrip` is normal aligned RAM.
- **OBJ budget**: `MaxStatusObjVramBytes` now derives 512 bytes (`64x16`), 256 bytes (`24x16`), 768 bytes (`66` rounded to `96x16`), and 640 bytes of speaker/battery graphics: 2176 bytes total. The host test derives this sum rather than trusting stale `1920`.
- **Historical status — superseded**: At this remediation stage, Medium10 measured baseline/centering, nickname `[190,256)`, and speaker/battery visibility were preserved. This historical state was superseded by the completed reproducible Medium9 status-font remediation; current/final status behavior uses Medium9. No whole-strip DMA or row-spanning generation was introduced beyond the retained c118 dirty-row behavior at that stage.
- **Physical status**: c118 itself occasionally showed text cuts. This correction restores the physically usable launcher-visibility/stability strategy only; it does not claim no tearing. The physical matrix remains failed/unaccepted and task 3.3 remains unchecked.

## TDD Cycle Evidence — vblank-starvation-remediation
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| vblank-starvation-remediation | `tests/status_strip_compositor_tests.cpp`, `tests/status_bar_format_tests.cpp` | Unit/source contract | ✅ both focused executables exit 0 before edits | ✅ Removed atomic publication contract; added derived rounded-label OBJ-budget and bounded dirty-row/cache-flush contracts. Format test compile exit 1 (`StatusLabelObjVramBytes` missing; stale 1920 failed). | ✅ both focused executables exit 0 after minimal changes | ✅ 64/24/66px label widths plus 132-byte one-row and sub-strip upload cases | ✅ Deleted atomic state machine; extracted one pure rounded-label-byte calculation |

## Work Unit Evidence — vblank-starvation-remediation
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests` — exit 0; 2 executables passed. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4/4 passed. |
| Runtime harness command/scenario and exact result | `podman run --rm --entrypoint /bin/sh -v /tmp/pico-launcher-vblank-starvation-remediation:/workspace:Z -w /workspace docker.io/skylyrac/blocksds:slim-v1.16.0 -c 'make clean && make -j1'`, then `make -j1` — exit 0; clean serial BlocksDS v1.16.0 build. |
| ROM identity | `/tmp/pico-launcher-vblank-starvation-remediation/LAUNCHER.nds`, 523776 bytes, `sha256:960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`. |
| Diff/process | `git diff --check` — exit 0. Container used `--rm`; no build child remained. No SD install, hardware claim, commit, push, PR, archive, or `.atl` mutation. |
| Rollback boundary | Revert only the a75a-to-c118 publication delta in `StatusBackgroundContracts.h`, `CustomSubBackground.{h,cpp}`, `StatusBarFormat.h`, and the two focused test files; this preserves all unrelated status composition work. |

```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:d200508e4908e54bc7bfdf281bd34381491072f3dfda6458c7afb743cba50ac1
failed_evidence_revision: sha256:ed6f4f561c145671b2f3d8edbf737803d545dbaf73be1bf4c11bc3f5e5e58a3a
lineage_id: physical-defect-remediation
generation: 1
fix_batch: 3
change: compose-status-text-on-theme-background
result: corrected_pending_hardware_retest
native_attempt_token: sha256:c1e88c80e276e43f9c6b58509aec3819f991d764bb86309e34d139f00c936184
native_attempt_acquired: false
native_attempt_settled: false
physical_matrix: failed_unaccepted
task_3_3_completed: false
```

## Launcher-Only DSteam Installation Evidence — Bounded VBlank Remediation
- Native token `sha256:a098557b297e4a4038fc3a01853d5fc55c02913b2d4cf73ab361bb7f4bb585e9` was neither acquired nor settled.
- Source was verified before and after installation without rebuilding: `/tmp/pico-launcher-vblank-starvation-remediation/LAUNCHER.nds`, 523776 bytes, `sha256:960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`.
- Independent identity gates agreed: mounted `/dev/sda1`, rw `vfat`, UUID `0BFC-CD24`; UDisks reported `IdUUID=0BFC-CD24`, `ReadOnly=false`; USB serial was `000000001536`.
- Predecessor gate passed: `/_picoboot.nds`, 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- Verified backup: `/_picoboot.nds.backup-pre-vblank-remediation-20260814T014510Z`, 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- Same-directory staging, atomic replacement, file/directory fsync, `syncfs`, and `sync` completed. Readback was 523776 bytes, `sha256:960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d`. The root transaction boundary was launcher replacement plus the verified backup; no staging path remained.
- UDisks unmount succeeded; independent `findmnt` absence and UDisks empty `MountPoints` confirmed absence.
- Physical matrix remains failed/unaccepted; task 3.3 remains unchecked. No no-tearing or physical-acceptance claim. No source, test, task-checkbox, commit, push, or PR change occurred.

## Installation Work Unit Evidence — Bounded VBlank Remediation
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | Source and predecessor byte/SHA-256 gates — exit 0; matched exact requested identities. |
| Runtime harness command/scenario and exact result | Same-directory atomic FAT launcher-only installer — exit 0; backup/readback verification, file/directory fsync, `syncfs`, `sync`, temporary-path absence, and UDisks unmount with independent absence checks passed. |
| Rollback boundary | Atomically restore only `/_picoboot.nds` from `/_picoboot.nds.backup-pre-vblank-remediation-20260814T014510Z`. |

```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:13c6dcf3deeb5d7f2e3a1d572ac9936dbf4dae222b04d3ceb35d024e74d83207
change: compose-status-text-on-theme-background
result: success
native_attempt_token: sha256:a098557b297e4a4038fc3a01853d5fc55c02913b2d4cf73ab361bb7f4bb585e9
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
media_usb_serial: 000000001536
target: /_picoboot.nds
predecessor_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
backup: /_picoboot.nds.backup-pre-vblank-remediation-20260814T014510Z
backup_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
candidate_sha256: 960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d
readback_sha256: 960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d
root_diff: launcher_replaced_plus_one_verified_backup
durability: staged_file_fsync_atomic_rename_installed_file_fsync_directory_fsync_syncfs_sync
unmount_proven: true
physical_matrix: failed_unaccepted
task_3_3_completed: false
no_tearing_claim: false
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:d200508e4908e54bc7bfdf281bd34381491072f3dfda6458c7afb743cba50ac1","failed_evidence_revision":"sha256:ed6f4f561c145671b2f3d8edbf737803d545dbaf73be1bf4c11bc3f5e5e58a3a","lineage_id":"physical-defect-remediation","generation":1,"fix_batch":3,"focused_cpp":"pass: 2 executables","targeted_python":"pass: 4/4","blocksds":"pass: BlocksDS v1.16.0","rom":{"path":"/tmp/pico-launcher-vblank-starvation-remediation/LAUNCHER.nds","bytes":523776,"sha256":"960d5cf00d2f6e258c97828630424f22b54dd8e84a0cc0839ad310021266e96d"},"hardware":"failed_unaccepted; no no-tearing claim"}
```

## Hardware Defect Remediation — physical-defect-remediation
- Native attempt token: `sha256:873cec9b8d792a1b81f8af5e7f39edd438dca700693888bc546f9bf41834eee4` was neither acquired nor settled.
- **Diagnosis**: custom composition repeatedly DMA-published the entire 8 KiB strip in VBlank without a completion wait or a VBlank-time budget. This allowed a scanout-visible partial write. Separately, `Draw()` put both OBJ icons inside the non-composed text branch, so composed custom mode suppressed valid speaker/battery graphics together with text labels.
- **Fix**: composition completes in RAM, coalesces a clipped dirty rectangle, and publishes only its complete rows when the bounded transfer fits before VBlank end; each DMA waits before the next row and the pending rectangle is retained for the next VBlank otherwise. Restore includes the last published rectangle. Text remains hidden only in `Composed`; icons draw whenever graphics are ready, with existing valid-volume/NTR semantics.
- **Physical status**: reinstall and covered hardware retest required. Matrix remains `0/20`, unaccepted; task 3.3 remains unchecked.

## TDD Cycle Evidence — Hardware Defect Remediation
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| physical-defect-remediation | `tests/status_strip_compositor_tests.cpp`, `tests/status_bar_format_tests.cpp` | Unit/source contract | ✅ Both focused executables passed before changes | ✅ Missing dirty upload plan and composed-icon truth-table contracts; each focused compile failed | ✅ Both focused executables exit 0 | ✅ Valid/clipped/invalid upload plans; composed/material/fallback icon modes | ✅ Minimal pure plan and shared icon predicate |

## Work Unit Evidence — Hardware Defect Remediation
| Evidence | Exact result |
|---|---|
| Focused C++ | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/status-strip-compositor-tests && /tmp/status-strip-compositor-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests` — exit 0; 2 executables passed. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4 passed. |
| Runtime harness | `podman run --rm --entrypoint /bin/sh -v /tmp/pico-launcher-hardware-defect-remediation:/workspace:Z -w /workspace docker.io/skylyrac/blocksds:slim-v1.16.0 -c 'make clean && make -j1'` — exit 0; clean serial BlocksDS v1.16.0 build. |
| ROM identity | `/tmp/pico-launcher-hardware-defect-remediation/LAUNCHER.nds`, 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`. |
| Cleanup/process | Container used `--rm`; retained build copy is isolated under `/tmp`; no install, commit, push, PR, `.atl`, or asset mutation. `git diff --check` exit 0. |
| Rollback | Revert only `StatusBackgroundContracts.h`, `CustomSubBackground.{h,cpp}`, `StatusBarFormat.h`, `RomBrowserTopScreenView.cpp`, and the two focused host tests; restores prior full-strip publication and icon branch behavior without unrelated work. |

```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:4c17b6ac17c83aaf84f4c71af31da17ef07f4d10288d2c18e03794a3c6be4ca1
failed_evidence_revision: sha256:355c8bf0cbb537f896966359ecbb3b490e52a1320527ab6566617c7596caeb05
lineage_id: physical-defect-remediation
generation: 1
fix_batch: 1
change: compose-status-text-on-theme-background
result: corrected_pending_hardware_retest
native_attempt_token: sha256:873cec9b8d792a1b81f8af5e7f39edd438dca700693888bc546f9bf41834eee4
native_attempt_acquired: false
native_attempt_settled: false
physical_matrix: 0/20_unaccepted
task_3_3_completed: false
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:4c17b6ac17c83aaf84f4c71af31da17ef07f4d10288d2c18e03794a3c6be4ca1","failed_evidence_revision":"sha256:355c8bf0cbb537f896966359ecbb3b490e52a1320527ab6566617c7596caeb05","lineage_id":"physical-defect-remediation","generation":1,"fix_batch":1,"focused_cpp":"pass","targeted_python":"4/4 pass","blocksds":"pass","rom":{"path":"/tmp/pico-launcher-hardware-defect-remediation/LAUNCHER.nds","bytes":523776,"sha256":"c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f"},"hardware":"reinstall_and_retest_required"}
```

## Launcher-Only DSteam Installation Evidence
- Native attempt token: `sha256:ebe1bf98f7baf43f95b876a7cc7ac3327cef5efee3ade274d534a151bdbfd67f` was neither acquired nor settled by this executor.
- Exact retained source: `/tmp/pico-launcher-blocksds-remediation/LAUNCHER.nds`, 523264 bytes, `sha256:551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f`; no rebuild occurred.
- Before mutation, the mounted `vfat` filesystem at `/run/media/guill3/0BFC-CD24` was independently corroborated as `/dev/sda1` by `findmnt`, udev `ID_FS_UUID=0BFC-CD24`, and UDisks `IdUUID=0BFC-CD24`, `IdType=vfat`, `ReadOnly=false`; the mount was `rw`.
- Root pre-state listing hash: `sha256:b527b22b590b4e92e9fc66b244d82155ebdf9e4ae28564036662fc2aa8f3c515`. Expected predecessor `/_picoboot.nds` was verified: 519680 bytes, `sha256:d3c2e5bebbaa4212dee332d9a5b58b238ecebc4a24a115cd7523e1969a1f0f05`.
- Verified backup: `/_picoboot.nds.backup-pre-composed-status-text-20260813T205019Z`, 519680 bytes, `sha256:d3c2e5bebbaa4212dee332d9a5b58b238ecebc4a24a115cd7523e1969a1f0f05`.
- The candidate was staged and atomically renamed in the target directory. File and directory fsync completed, followed by `sync`; exact `/_picoboot.nds` readback was 523264 bytes, `sha256:551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f`.
- Root post-state listing hash: `sha256:42514a787d7d5c668d43e42bfeca74fecb74ecd752bd80fb732e376719d31618`. Its only changes are launcher replacement and the one verified backup addition; no temporary file remained.
- `udisksctl unmount -b /dev/sda1` succeeded. Independent `findmnt` absence and UDisks empty `MountPoints` prove it is unmounted.
- Source/test/task checkbox files were not edited. Repository HEAD remained `36a3d30b501de9bf3775a6b1898a377bd3bed172`; its existing dirty status was unchanged by this media transaction.

## Physical Matrix Status
- Physical matrix remains `0/20`, unobserved. This is installation preparation only: task 3.3 stays unchecked and no physical acceptance is claimed.

## Remediation Work Unit — profile-gap-eight
- **Historical geometry — superseded**: This was the final approved geometry at that stage: `StatusBatteryGap` changed from 4px to 8px, nickname ink remained right-anchored at x=256 with no inset or avatar reservation, and `MakeBatteryBounds` used ellipsis-aware rendered nickname width. The historical exact bounds were empty `[232,248)`, short 10px `[222,238)`, maximum/overlong `[166,182)`; matching nickname ink `[256,256)`, `[246,256)`, `[190,256)`; every battery-to-ink gap was 8px.
- This historically approved geometry was later superseded by the final accepted `StatusNicknameRight=256` and configured `StatusBatteryGap=18`, with battery coordinates preserved and DSi/NTR safety clamps. The configured gap does not claim every effective gap is 18px.
- Date remains `[96,160)` centered at x=128; speaker remains y `[0,16)` and battery y `[4,12)`. No compositor, colors, icons, font, coverage capacity, or VBlank path changed.
- Task 3.3 and the physical 5×4 matrix remain pending/unaccepted. No SD/hardware operation, commit, push, PR, or archive action occurred.

## TDD Cycle Evidence — profile-gap-eight
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| profile-gap-eight | `tests/status_bar_format_tests.cpp` | Unit | Existing focused executable exit 0 | Exact 8px static assertions failed: prior gap reduced to 4 | Focused format and compositor executables exit 0 after one shared-constant change | Empty, short, maximum, and overlong nicknames; right x=256; date and vertical bounds; non-overlap | None needed — reused `StatusBatteryGap`/bounds helpers |

## Work Unit Evidence — profile-gap-eight
| Evidence | Exact result |
|---|---|
| Focused C++ | Format and compositor host executables — exit 0; 2 passed. |
| Coverage/canary | Temporary NDS typedef shim plus current renderer body — exit 0; 64/24/66 coverage paths and guard canaries passed. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4/4 passed. |
| Runtime harness | Clean serial BlocksDS v1.16.0 build in `/tmp/pico-launcher-profile-gap-eight` — exit 0 after the initial clean build reached the 120s wrapper timeout at `NDSTOOL`; completion command exited 0. |
| ROM | `/tmp/pico-launcher-profile-gap-eight/LAUNCHER.nds`, 542208 bytes, `sha256:4e0e519d33b25a646f7b3174e094ecfdb375863b7c42a307f09827dbf3d5e735`. |
| Diff | `git diff --check` — exit 0. |
| Rollback | Revert the one `StatusBatteryGap` value and these focused assertions; unrelated status work stays intact. |

```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:250f7c959c3ebc5d257e21cbdf20cd61abf9f7d0059577630d7d9d164e87178e
settlement_evidence_revision: sha256:68a14edc5e79ceda1105f447eddef4b7ef0a0ec13d4278af5383847a3acda25f
change: compose-status-text-on-theme-background
work_unit: profile-gap-eight
result: corrected_pending_hardware_retest
native_attempt_token: sha256:9055465cd4d6ba89b9765d86c446903ab8ef82aa5aa6e7d48093e925fdbff349
native_attempt_acquired: false
native_attempt_settled: false
physical_matrix: pending_unaccepted
task_3_3_completed: false
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:250f7c959c3ebc5d257e21cbdf20cd61abf9f7d0059577630d7d9d164e87178e","settlement_evidence_revision":"sha256:68a14edc5e79ceda1105f447eddef4b7ef0a0ec13d4278af5383847a3acda25f","change":"compose-status-text-on-theme-background","work_unit":"profile-gap-eight","native_attempt_token":"sha256:9055465cd4d6ba89b9765d86c446903ab8ef82aa5aa6e7d48093e925fdbff349","native_attempt_acquired":false,"native_attempt_settled":false,"focused_cpp":"pass: 2 executables","coverage_canary":"pass: 64/24/66 paths and guard canaries","targeted_python":"pass: 4/4","blocksds":"pass: BlocksDS v1.16.0","rom":{"path":"/tmp/pico-launcher-profile-gap-eight/LAUNCHER.nds","bytes":542208,"sha256":"4e0e519d33b25a646f7b3174e094ecfdb375863b7c42a307f09827dbf3d5e735"},"hardware":"pending_unaccepted; no physical acceptance claim","task_3_3_completed":false}
```

## Result Contract
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:52ede9701ad48d01c5730fd0d8445ca12e80be3fdd27676a6b95637fd2168a07
change: compose-status-text-on-theme-background
result: success
native_attempt_token: sha256:ebe1bf98f7baf43f95b876a7cc7ac3327cef5efee3ade274d534a151bdbfd67f
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
target: /_picoboot.nds
predecessor_sha256: d3c2e5bebbaa4212dee332d9a5b58b238ecebc4a24a115cd7523e1969a1f0f05
backup: /_picoboot.nds.backup-pre-composed-status-text-20260813T205019Z
candidate_sha256: 551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f
readback_sha256: 551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f
root_diff: launcher_replaced_plus_one_verified_backup
unmount_proven: true
physical_matrix: 0/20_unobserved
task_3_3_completed: false
```

## Hardware-Approved Final Status Layout — status-horizontal-alignment
- Native attempt token `sha256:e3c581b658a5afa987f0aef7c6e568299625c347f1ae54b61449c099c78173c3` was neither acquired nor settled.
- Date/time bounds changed from DSi `[86,150)` / NTR `[73,137)` to `[96,160)` in both modes, centering the 64px label at x=128. Fallback uses the same date bounds.
- The nickname capacity remains `[190,256)` and its rendered right edge remains x=256. The dynamic group measures the post-substitution, ellipsis-aware `LabelView` rendered width, clamps it to 66px, and packs the battery 4px before its actual ink: short width 10 `[226,242)` then ink `[246,256)`; maximum/overlong `[170,186)` then ink `[190,256)`; empty `[236,252)` then zero-width ink at x=256.
- Speaker remains `[2,18) × [0,16)` and battery remains 16×8 at y `[4,12)`. The composed, material-OBJ, and binary fallback paths share `RefreshStatus` layout state; no avatar reservation was restored.
- Physical matrix remains pending/unaccepted and task 3.3 remains unchecked. No SD/hardware operation, commit, push, PR, archive, asset/font/color/metric change, or `.atl` mutation was performed.

## TDD Cycle Evidence — status-horizontal-alignment
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| status-horizontal-alignment | `tests/status_bar_format_tests.cpp` | Unit/source contract | ✅ Existing focused status-format executable exit 0 before edits | ✅ Added absent `MakeNicknameInkBounds`/`MakeBatteryBounds` contracts; compile exit 1 and stale date center asserted x=118 | ✅ Focused status-format and compositor executables exit 0 | ✅ Short (10px), maximum (66px), empty, and overlong nickname cases; exact 4px gaps and no date overlap | ✅ Centralized screen/date/gap constants and one shared rendered-width accessor |

## Work Unit Evidence — status-horizontal-alignment
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests` — exit 0; 2 executables passed. |
| Coverage/canary command and exact result | Host compatibility command compiled `tests/nft2_coverage_renderer_tests.cpp` with the current `Nft2CoverageRenderer.cpp` body and temporary `/tmp` NDS typedef shim; `/tmp/pico-launcher-nft2-coverage-renderer-tests` exited 0. It proved 64/24/66px coverage paths and guard canaries. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4/4 passed. |
| Runtime harness command/scenario and exact result | `podman run --rm --entrypoint /bin/sh -v /tmp/pico-launcher-status-horizontal-alignment:/workspace:Z -w /workspace docker.io/skylyrac/blocksds:slim-v1.16.0 -c 'make clean && make -j1'`, then `make -j1` after the first command exceeded the shell timeout at `NDSTOOL` — final serial BlocksDS v1.16.0 build exit 0. |
| ROM identity | `/tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds`, 542208 bytes, `sha256:c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`. |
| Normalization and process | Final source newline normalization ran before verification; no formatter is configured. `git diff --check` — exit 0. Container used `--rm`; no installation, hardware claim, commit, push, PR, or archive action occurred. |
| Rollback boundary | Revert only `StatusBarFormat.h` shared geometry helpers, `LabelView.h` rendered-width accessor, `RomBrowserTopScreenView.cpp` dynamic group placement, and status-format test additions; font, colors, assets, VBlank/composition, file-info view, and unrelated drift remain intact. |

## Result Contract — status-horizontal-alignment
```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:ebb2d338f7edfe4c73af92ed24a2707b15de1f57a5eec8caae540ebbc0e2e8b6
settlement_evidence_revision: sha256:779963c83b9f0536d2477037c5cb73ecb937a824f8f0361eb5b5365a7fcafd8e
change: compose-status-text-on-theme-background
work_unit: status-horizontal-alignment
result: corrected_pending_hardware_retest
native_attempt_token: sha256:e3c581b658a5afa987f0aef7c6e568299625c347f1ae54b61449c099c78173c3
native_attempt_acquired: false
native_attempt_settled: false
physical_matrix: pending_unaccepted
task_3_3_completed: false
rom:
  path: /tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds
  bytes: 542208
  sha256: c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9
```

## Hardware Defect Remediation — profile-gap-eight (NTR clamp)
- Native attempt token `sha256:7a27d491063d7d83ce6bd22c17fb4fa7efe45fdfd1894a8fb34636a1fc467d30` was neither acquired nor settled.
- **Diagnosis**: the approved DSi dynamic battery geometry produced `[166,182)` for a maximum/overlong nickname, which overlaps unchanged NTR `HIGH`/`LOW` bounds `[144,168)` by 2px.
- **Correction**: the shared dynamic `MakeBatteryBounds(dsiMode, displayedNicknameWidth)` now clamps the NTR battery left edge to `StatusNtrTextRight` (168). `RefreshStatus` supplies the active mode to the same helper used by layout, composed, material-OBJ, and binary fallback paths; no separate rendering geometry exists.
- **Preserved DSi geometry**: nickname ink remains right-anchored at x=256 with no inset. Empty `[256,256)` / battery `[232,248)`, short 10px `[246,256)` / battery `[222,238)`, maximum and overlong `[190,256)` / battery `[166,182)` retain an exact 8px gap.
- **NTR geometry**: NTR text remains `[144,168)`; short nickname uses battery `[222,238)` and retains 8px. Maximum and overlong nickname use battery `[168,184)`, have no overlap, and retain the largest safe 6px gap. All asserted battery bounds are on-screen.
- Date/time `[96,160)`, NTR text/date/font, speaker bounds `[2,18) × [0,16)`, battery y `[4,12)`, colors, icons, coverage, and VBlank behavior are unchanged. Task 3.3 and the physical matrix remain pending/unaccepted. No SD/hardware operation, commit, push, PR, or archive action occurred.

## TDD Cycle Evidence — profile-gap-eight (NTR clamp)
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| profile-gap-eight | `tests/status_bar_format_tests.cpp` | Unit | Existing focused format executable exit 0 | Added mode-aware `MakeBatteryBounds(bool, int)` contracts; compile exit 1 because the previous helper accepted only one argument | Focused format and compositor executables exit 0 after the minimal mode-aware shared clamp | DSi empty/short/maximum/overlong 8px cases; NTR short 8px; NTR maximum/overlong non-overlap and 6px safe gap; on-screen bounds | Added `StatusNtrTextRight` so layout and dynamic placement share the NTR edge; tests remained green |

## Work Unit Evidence — profile-gap-eight (NTR clamp)
| Evidence | Exact result |
|---|---|
| Focused C++ | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests` — exit 0; 2 executables passed. |
| Coverage/canary | Current renderer body with the temporary NDS typedef shim — exit 0; 64/24/66 paths and guard canaries passed. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4/4 passed. |
| Runtime harness | `podman run --rm --entrypoint /bin/sh -v /tmp/pico-launcher-profile-gap-eight:/workspace:Z -w /workspace docker.io/skylyrac/blocksds:slim-v1.16.0 -c 'make clean && make -j1'` — exit 0; clean serial BlocksDS v1.16.0 build. |
| ROM | `/tmp/pico-launcher-profile-gap-eight/LAUNCHER.nds`, 542208 bytes, `sha256:d2a7049c03ad91cc9f6cc8e2b9a986de6d66426b29279474ddfa32d0c1b7fbc7`. |
| Diff | `git diff --check` — exit 0. |
| Rollback | Revert the mode-aware `MakeBatteryBounds`/`MakeBatteryStateBounds` calls, `StatusNtrTextRight`, and this unit's assertions in `StatusBarFormat.h`, `RomBrowserTopScreenView.cpp`, and `status_bar_format_tests.cpp`; unrelated status composition remains intact. |

```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:1c8ebe2924411409b54b0b1bc78c1d8abf07b077ab0a58640efa6eddd2509c98
settlement_evidence_revision: sha256:128593b40f8569f3036f5692345ba089e6dc27508f2648ae458414cd23280a64
failed_evidence_revision: sha256:68a14edc5e79ceda1105f447eddef4b7ef0a0ec13d4278af5383847a3acda25f
lineage_id: profile-gap-eight
generation: 1
fix_batch: 2
change: compose-status-text-on-theme-background
work_unit: profile-gap-eight
result: corrected_pending_hardware_retest
native_attempt_token: sha256:7a27d491063d7d83ce6bd22c17fb4fa7efe45fdfd1894a8fb34636a1fc467d30
native_attempt_acquired: false
native_attempt_settled: false
physical_matrix: pending_unaccepted
task_3_3_completed: false
rom:
  path: /tmp/pico-launcher-profile-gap-eight/LAUNCHER.nds
  bytes: 542208
  sha256: d2a7049c03ad91cc9f6cc8e2b9a986de6d66426b29279474ddfa32d0c1b7fbc7
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:1c8ebe2924411409b54b0b1bc78c1d8abf07b077ab0a58640efa6eddd2509c98","settlement_evidence_revision":"sha256:128593b40f8569f3036f5692345ba089e6dc27508f2648ae458414cd23280a64","failed_evidence_revision":"sha256:68a14edc5e79ceda1105f447eddef4b7ef0a0ec13d4278af5383847a3acda25f","change":"compose-status-text-on-theme-background","work_unit":"profile-gap-eight","lineage_id":"profile-gap-eight","generation":1,"fix_batch":2,"native_attempt_token":"sha256:7a27d491063d7d83ce6bd22c17fb4fa7efe45fdfd1894a8fb34636a1fc467d30","native_attempt_acquired":false,"native_attempt_settled":false,"focused_cpp":"pass: 2 executables","coverage_canary":"pass: 64/24/66 paths and guard canaries","targeted_python":"pass: 4/4","blocksds":"pass: BlocksDS v1.16.0","rom":{"path":"/tmp/pico-launcher-profile-gap-eight/LAUNCHER.nds","bytes":542208,"sha256":"d2a7049c03ad91cc9f6cc8e2b9a986de6d66426b29279474ddfa32d0c1b7fbc7"},"physical_matrix":"pending_unaccepted","task_3_3_completed":false}
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:ebb2d338f7edfe4c73af92ed24a2707b15de1f57a5eec8caae540ebbc0e2e8b6","settlement_evidence_revision":"sha256:779963c83b9f0536d2477037c5cb73ecb937a824f8f0361eb5b5365a7fcafd8e","change":"compose-status-text-on-theme-background","work_unit":"status-horizontal-alignment","native_attempt_token":"sha256:e3c581b658a5afa987f0aef7c6e568299625c347f1ae54b61449c099c78173c3","native_attempt_acquired":false,"native_attempt_settled":false,"focused_cpp":"pass: 2 executables","coverage_canary":"pass: 64/24/66 paths and guard canaries","targeted_python":"pass: 4/4","blocksds":"pass: BlocksDS v1.16.0","rom":{"path":"/tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds","bytes":542208,"sha256":"c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9"},"hardware":"pending_unaccepted; no physical acceptance claim","task_3_3_completed":false}
```

## Emergency Hardware Rollback — Rejected Centered Atomic Status
- Native token `sha256:a8b7354fd390a679ceb296539afd48584870c5d2c4391bdf4a661152374475f5` was neither acquired nor settled.
- Hardware rejected `sha256:a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2`; the physical matrix is failed/unaccepted and task 3.3 remains unchecked.
- Independent media identity gates agreed: UUID `0BFC-CD24` resolved to `/dev/sda1`, mounted rw as `vfat` at `/run/media/guill3/0BFC-CD24`; UDisks reported the same UUID, device, type, and mount.
- Pre-state target `/_picoboot.nds` was exactly 523776 bytes, `sha256:a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2`.
- Rejected candidate preserved as `/_picoboot.nds.backup-rejected-centered-atomic-20260814T010833Z`, exactly 523776 bytes with the same rejected SHA-256.
- Stable rollback source remained untouched: `/_picoboot.nds.backup-pre-centered-atomic-status-20260813T230814Z`, exactly 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- Same-directory staging, file fsync, atomic rename, restored-file and root-directory fsync, `syncfs`, and `sync` completed. Exact target readback is 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- Root proof: exactly one new rejected-backup path, launcher replacement only, and no transaction temporary artifacts.
- `udisksctl unmount -b /dev/sda1` returned `Unmounted /dev/sda1.` UUID absence from `findmnt`, device absence from `/proc/mounts`, and UDisks empty `MountPoints` independently prove unmount.
- No source, test, task-checkbox, commit, push, or PR changes occurred.

## Work Unit Evidence — Emergency Hardware Rollback
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | UUID, UDisks, target, and rollback-source byte/SHA-256 identity gates passed before mutation. |
| Runtime harness command/scenario and exact result | Atomic FAT launcher rollback completed; exact c118 readback, file/directory fsync, `syncfs`, `sync`, root-diff gate, UDisks unmount, and three independent absence checks passed. |
| Rollback boundary | Atomically restore only `/_picoboot.nds` from the preserved rejected backup; the stable rollback source remains untouched. |

```yaml
schema: gentle-ai.emergency-rollback-result/v1
evidence_revision: sha256:d1c5feeb8fc2f5c7d688fd5dc58e8911b49fca0cc24dc057d636475043c23039
change: compose-status-text-on-theme-background
result: success
native_attempt_token: sha256:a8b7354fd390a679ceb296539afd48584870c5d2c4391bdf4a661152374475f5
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device_by_uuid: /dev/sda1
target: /_picoboot.nds
rejected_pre_state_sha256: a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2
rejected_backup: /_picoboot.nds.backup-rejected-centered-atomic-20260814T010833Z
rollback_source: /_picoboot.nds.backup-pre-centered-atomic-status-20260813T230814Z
restored_readback_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
root_diff: launcher_replaced_plus_one_rejected_backup
unmount_proven: true
physical_matrix: failed_unaccepted
task_3_3_completed: false
```

## Preservation and Rollback Audit (Task 4.1)
- Candidate scope excludes unrelated `.atl` drift, archives, current capability specs, generic OpenSpec scaffolding, commits, PRs, and hardware changes. The only `arm9/gfx` delta is the intentional pre-existing removal of obsolete `statusProfile.grit` and `statusProfile.png`; no asset bytes were added or modified.
- Forced unsupported contract: `CustomCompositionMode(false, true)` and `MaterialCompositionMode(true, false)` both select `StatusPresentationMode::Unsupported`; `StatusTextMode(false, false)` selects `BinaryFallback`, and `StatusTextVisible(BinaryFallback, true)` keeps binary text visible.
- Pristine restoration contract: after a composed strip is changed, a zero-coverage compose restores `destination == pristine`; `RebuildCustomStrip(pristineStrip, composedStrip)` restores the full strip before unsupported fallback.
- No production or test bytes were changed for this audit. Existing host tests already exercise the required forced-unsupported, binary fallback, and restoration contracts; therefore no additional assertion was necessary.

## TDD Cycle Evidence
| Task | RED | GREEN | REFACTOR |
|---|---|---|---|
| 4.1 | N/A — preservation audit uses pre-existing host contract cases; no production behavior was added. | Focused C++ host contracts passed: 2 executables, exit 0. | None — no code change required. |

## Work Unit Evidence (Task 4.1)
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests` — exit 0; 2 executables passed. The first proves composed-to-pristine restoration and forced `Unsupported`; the second proves binary fallback visibility and DSi/NTR textual contracts. |
| Targeted dependency-free Python assets | `python3 tests/test_status_bar_assets.py` — exit 0; 4 tests passed. |
| Runtime harness command/scenario and exact result | N/A — no product bytes changed in this preservation-only unit; the previously recorded clean BlocksDS build remains applicable and was not rerun. |
| Candidate scope and process | `git diff --check` — exit 0. `git diff --name-only -- .atl openspec/changes/archive openspec/specs` shows only pre-existing `.atl` drift; no archive/current-capability changes. No commit, PR, hardware, SD, `make`, `grit`, or `podman` process was created; process check found none. |
| Rollback boundary | Revert only this task's two OpenSpec artifacts (`tasks.md` checkbox and this audit section in `apply-progress.md`); product behavior and all unrelated drift remain untouched. Product rollback remains: force `Unsupported`, call `RestoreStatusStrip`, and retain binary date/time, NTR, nickname, and DSi fallback text. |

## Remaining
- Historical limitation: the 20-cell matrix was unobserved at this stage and remains **NOT EXECUTED** and **NOT PASSED** under the final limited-scope close.


## Launcher-Only DSteam Installation Evidence — Atomic Status Icons
- Native attempt token: `sha256:77d344fc4d5dc9cab6c9a3bb4fcb15b22cecf5eb1d276adaf65e7e9bbacd63b8` was neither acquired nor settled.
- Exact source was verified before media mutation and retained unchanged: `/tmp/pico-launcher-hardware-defect-remediation/LAUNCHER.nds`, 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`; no rebuild occurred.
- Independent media identity signals agreed before mutation: mount source `/dev/sda1` (`findmnt`), `/dev/disk/by-uuid/0BFC-CD24` resolving to `/dev/sda1`, and UDisks `IdUUID=0BFC-CD24`, `IdType=vfat`, `IdUsage=filesystem`, `HintSystem=false`. The mounted filesystem was read-write `vfat`.
- Expected predecessor was verified before mutation: `/_picoboot.nds`, 523264 bytes, `sha256:551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f`.
- Verified backup: `/_picoboot.nds.backup-pre-atomic-status-icons-20260813T213201Z`, 523264 bytes, `sha256:551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f`.
- The candidate was copied to a same-directory temporary file, file-fsynced, atomically renamed to `/_picoboot.nds`, then the installed file and mount directory were fsynced; `syncfs` and `sync` both completed. Exact installed readback: 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- Root diff was fail-closed: only launcher replacement plus that one verified backup addition; no root entries were removed and no temporary file remained.
- `udisksctl unmount -b /dev/sda1` succeeded. Independent absence proof: `findmnt -S /dev/sda1` returned no mount, `findmnt -T /run/media/guill3/0BFC-CD24` returned no mount, UDisks reported empty `MountPoints`, `lsblk` reported no mountpoint, and the former mount path was inaccessible.
- This is installation preparation only. Physical matrix remains `0/20` unobserved and task 3.3 remains unchecked; no physical acceptance is claimed. No source, test, task-checkbox, commit, push, or PR mutation occurred.

## Installation Work Unit Evidence
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | Source identity gate: `stat -c '%s' /tmp/pico-launcher-hardware-defect-remediation/LAUNCHER.nds && sha256sum /tmp/pico-launcher-hardware-defect-remediation/LAUNCHER.nds` — exit 0; 523776 bytes and exact candidate SHA-256. Predecessor identity gate — exit 0; 523264 bytes and exact expected SHA-256. |
| Runtime harness command/scenario and exact result | Atomic FAT launcher-only installation script — exit 0; verified backup, exact readback, root diff, file/directory fsync, `syncfs`, and `sync`. |
| Rollback boundary | Restore `/_picoboot.nds` from `/_picoboot.nds.backup-pre-atomic-status-icons-20260813T213201Z`; this changes only the launcher and retains every other root entry. |

## Installation Result Contract
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:5395230dcc7bb2fcbd184d7610d820a5040216be3010ca3c97d31fddf57f2c2d
change: compose-status-text-on-theme-background
result: success
native_attempt_token: sha256:77d344fc4d5dc9cab6c9a3bb4fcb15b22cecf5eb1d276adaf65e7e9bbacd63b8
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
target: /_picoboot.nds
predecessor_bytes: 523264
predecessor_sha256: 551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f
backup: /_picoboot.nds.backup-pre-atomic-status-icons-20260813T213201Z
backup_sha256: 551205eddeda781041e6b1d0296978e32e50f5f6f06e8275bb1238ef74f4808f
candidate_bytes: 523776
candidate_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
readback_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
root_diff: launcher_replaced_plus_one_verified_backup
durability: file_fsync_directory_fsync_syncfs_sync
unmount_proven: true
physical_matrix: 0/20_unobserved
task_3_3_completed: false
```

## Hardware Defect Remediation — physical-layout-remediation
- Native attempt token: `sha256:40174927d474b3707579df6f6caac1097e83f7e923fdf31f786fd3d6c447a555` was neither acquired nor settled.
- **Diagnosis**: Medium7_5 had a 10px line box rendered at y=0, leaving its actual ink high in the 16px strip. The first remediation's row-by-row dirty DMA could expose part of one logical composed generation before a later row or deferred remainder, so waiting each row did not guarantee atomic user-visible publication.
- **Font/vertical metrics**: inspected NFT2 metrics for Medium7_5 (ascend=8, descend=2), Regular10 (11,2), Medium10 (11,2), and Medium11 (12,3). Medium10 is the smallest next existing size after Medium7_5 that fits: `12/31 23:59`=57/64px, `HIGH`=24/24px, `LOW`=23/24px, and worst ellipsized nickname `WWWW ... W`=58/66px. Its baseline is `(16-11-2)/2=1`; representative ink ranges y=4..12. Medium11 has 15px metrics and does not improve the selected 57px date width, so it was not selected.
- **Geometry**: user-approved nickname bound changed from `[186,236)` to `[190,256)`: a 4px clear gap after unchanged battery `[170,186)`, 66px safe right-edge width, no profile tile. Date/time remains `[86,150)` DSi / `[73,137)` NTR; NTR remains `[144,168)`.
- **Historical atomic strategy — superseded**: At this remediation stage, each changed status generation was composed fully in RAM, then published with one waited contiguous 8KiB DMA only if four VBlank lines remained; otherwise the complete generation was deferred. This historical whole-strip publication, together with the Medium10/8px remediation state above, was later superseded by bounded dirty-row publication with an ARM9 DMA source cache flush. Current/final publication uses bounded dirty rows; covers and icons retain their existing order/semantics.
- **Physical status**: no install or hardware claim. Physical matrix remains `0/20` unaccepted; task 3.3 remains unchecked.

## TDD Cycle Evidence — physical-layout-remediation
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| physical-layout-remediation | `tests/status_bar_format_tests.cpp`, `tests/status_strip_compositor_tests.cpp` | Unit/source contract | ✅ both focused executables exit 0 before edits | ✅ missing metrics/baseline/worst-string geometry and atomic publication state contracts; both focused compiles exit 1 | ✅ focused C++ executables exit 0 | ✅ date/HIGH/LOW/nickname, font metrics, safe/unsafe VBlank generations | ✅ one pure baseline/fit contract and one publication state machine |

## Work Unit Evidence — physical-layout-remediation
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/status-strip-compositor-tests && /tmp/status-strip-compositor-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests` — exit 0; 2 executables passed. |
| Targeted Python | `python3 tests/test_status_bar_assets.py` — exit 0; 4 passed. |
| Runtime harness command/scenario and exact result | `podman run --rm --entrypoint /bin/sh -v /tmp/pico-launcher-physical-layout-remediation:/workspace:Z -w /workspace docker.io/skylyrac/blocksds:slim-v1.16.0 -c 'make -j1'` — exit 0 after clean serial build; BlocksDS v1.16.0 produced the identified ROM. |
| Process/cleanup | `git diff --check` — exit 0. Container used `--rm`; no `make`, `grit`, or `podman` process remained. Isolated reproducibility copy remains under `/tmp`; no install, commit, push, PR, archive, `.atl`, or unrelated drift was touched. |
| Rollback boundary | Revert this work unit's changes in `StatusBarFormat.h`, `RomBrowserTopScreenView.{h,cpp}`, `StatusBackgroundContracts.h`, `CustomSubBackground.{h,cpp}`, the two focused C++ tests, and this change's spec/design/task/progress records. This restores prior font/bounds and row-DMA behavior without touching speaker/battery or unrelated work. |

```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:ed6f4f561c145671b2f3d8edbf737803d545dbaf73be1bf4c11bc3f5e5e58a3a
failed_evidence_revision: sha256:4c17b6ac17c83aaf84f4c71af31da17ef07f4d10288d2c18e03794a3c6be4ca1
lineage_id: physical-defect-remediation
generation: 1
fix_batch: 2
change: compose-status-text-on-theme-background
result: corrected_pending_hardware_retest
native_attempt_token: sha256:40174927d474b3707579df6f6caac1097e83f7e923fdf31f786fd3d6c447a555
native_attempt_acquired: false
native_attempt_settled: false
physical_matrix: 0/20_unaccepted
task_3_3_completed: false
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:ed6f4f561c145671b2f3d8edbf737803d545dbaf73be1bf4c11bc3f5e5e58a3a","failed_evidence_revision":"sha256:4c17b6ac17c83aaf84f4c71af31da17ef07f4d10288d2c18e03794a3c6be4ca1","lineage_id":"physical-defect-remediation","generation":1,"fix_batch":2,"focused_cpp":"pass: 2 executables","targeted_python":"pass: 4/4","blocksds":"pass: BlocksDS v1.16.0","rom":{"path":"/tmp/pico-launcher-physical-layout-remediation/LAUNCHER.nds","bytes":523776,"sha256":"a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2"},"hardware":"reinstall_and_retest_required"}
```

## Launcher-Only DSteam Installation Evidence — Centered Atomic Status
- Native attempt token: `sha256:0e79cba39203a1d43fedc83b36cd2277ac8d632285de498b6bc7a5359bd77fc2` was neither acquired nor settled.
- Exact source was verified before mutation and preserved unchanged: `/tmp/pico-launcher-physical-layout-remediation/LAUNCHER.nds`, 523776 bytes, `sha256:a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2`; no rebuild occurred.
- Independent identity signals agreed before mutation: `/dev/disk/by-uuid/0BFC-CD24` resolved to `/dev/sda1`; `findmnt -S UUID=0BFC-CD24` reported `/dev/sda1` mounted at `/run/media/guill3/0BFC-CD24` as read-write `vfat`; UDisks reported `IdUUID=0BFC-CD24`, `IdType=vfat`, `IdUsage=filesystem`, and `ReadOnly=false`.
- Expected predecessor was verified before mutation: `/_picoboot.nds`, 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- Verified backup: `/_picoboot.nds.backup-pre-centered-atomic-status-20260813T230814Z`, 523776 bytes, `sha256:c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f`.
- The candidate was copied to a same-directory staging file, file-fsynced, identity-checked, and atomically renamed to `/_picoboot.nds`; installed file and mount directory fsync completed, followed by `syncfs` and `sync`. Exact readback: 523776 bytes, `sha256:a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2`.
- Root pre-state listing hash: `sha256:d2640c5c419fa197047bc6953af402c619da0cc910dbdea6408d001821ef3fde`. Root post-state listing hash: `sha256:d28bbda5f406bfc29deca39ba5f8e13daa9b5796c9633809fea33c2617fe00cb`. The fail-closed root diff contains only launcher replacement and the one verified backup; no staging file remained.
- `udisksctl unmount -b /dev/sda1` succeeded. Independent absence proof: `findmnt -S /dev/sda1` and `findmnt -T /run/media/guill3/0BFC-CD24` returned no mount; UDisks showed empty `MountPoints`; `lsblk` reported no mountpoint.
- This is installation preparation only. Physical matrix remains `0/20` unobserved and task 3.3 remains unchecked; no source, test, task-checkbox, commit, push, or PR mutation occurred.

## Installation Work Unit Evidence — Centered Atomic Status
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `stat -c '%s' /tmp/pico-launcher-physical-layout-remediation/LAUNCHER.nds && sha256sum /tmp/pico-launcher-physical-layout-remediation/LAUNCHER.nds` — exit 0; 523776 bytes and exact candidate SHA-256. Predecessor identity gate — exit 0; 523776 bytes and exact expected SHA-256. |
| Runtime harness command/scenario and exact result | Same-directory FAT atomic launcher-only installer — exit 0; independently identified mounted media, verified predecessor and backup, staged-file fsync, atomic rename, installed-file/directory fsync, `syncfs`, `sync`, exact readback, root-diff gate, and UDisks unmount with independent absence checks. |
| Rollback boundary | Restore `/_picoboot.nds` from `/_picoboot.nds.backup-pre-centered-atomic-status-20260813T230814Z`; this changes only the launcher and retains every other root entry. |

## Installation Result Contract — Centered Atomic Status
```yaml
schema: gentle-ai.installation-result/v1
evidence_revision: sha256:f98f83ca515f0d92b95d793480cc3d815dd99cfb43f1018cfcb5f63e84390c70
change: compose-status-text-on-theme-background
result: success
native_attempt_token: sha256:0e79cba39203a1d43fedc83b36cd2277ac8d632285de498b6bc7a5359bd77fc2
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
target: /_picoboot.nds
predecessor_bytes: 523776
predecessor_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
backup: /_picoboot.nds.backup-pre-centered-atomic-status-20260813T230814Z
backup_sha256: c118502a806513562636ed9f6ac41042636d16177ec2f243bc5d8021ec97d85f
candidate_bytes: 523776
candidate_sha256: a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2
readback_sha256: a75a15c7bc4998e78b4e3c28f6fd19cf472f50a5f25331d3fa9b0c66ec60e8f2
root_pre_hash: d2640c5c419fa197047bc6953af402c619da0cc910dbdea6408d001821ef3fde
root_post_hash: d28bbda5f406bfc29deca39ba5f8e13daa9b5796c9633809fea33c2617fe00cb
root_diff: launcher_replaced_plus_one_verified_backup
durability: staged_file_fsync_atomic_rename_installed_file_fsync_directory_fsync_syncfs_sync
unmount_proven: true
physical_matrix: 0/20_unobserved
task_3_3_completed: false
```

## Work Unit — medium9-status-font (attempt 2)
- Native attempt token `sha256:ac561170a782ac96fa69333e67498a34f4178b2f17fd5c9d1f4ae1456429138f` was neither acquired nor settled.
- Generated `arm9/data/NotoSansJP-Medium-9.nft2` from the official Noto Sans CJK 2.004 JP Medium source at commit `523d033d6cb47f4a80c58a35753646f5c3608a78`, Git blob `490cc708a90767e9a9c867e147df33a8c62c1f6d`, size 4,549,132 bytes, source SHA-256 `f396a3b57256e4515be9cb41f7aac54766d654890082a9f1b5c2451b5c093d8a`.
- `tools/generate_medium9_font.py` validates the source Git blob hash, preserves the Medium10 canonical 420-glyph character map byte-for-byte, emits NFT2 coverage nibbles with Pillow BASIC layout at 9px, and validates NFT2 bounds and sample widths. The cached OTF is external at `/tmp/pico-launcher-medium9-font-cache/NotoSansJP-Medium.otf`.
- Pinned renderer runtime: local image `localhost/pico-launcher-medium9-font:1` (`sha256:ab64b5d38a0342d75d2dc9163c7225e5f7d13e992ea3e9cfc39e9f000999c360`), built from `docker.io/library/python@sha256:47ae396f09c1303b8653019811a8498470603d7ffefc29cb07c88f1f8cb3d19f` with `Pillow==11.1.0` and wheel SHA-256 `9aa9aeddeed452b2f616ff5507459e7bab436916ccb10961c4a382cd3e03f47f`.
- Exact regeneration command:
  `podman run --rm -v /tmp/pico-launcher-medium9-generator:/workspace:ro,Z -v /tmp/pico-launcher-medium9-font-cache:/font-cache:Z -v /tmp/pico-launcher-medium9-a:/output:Z localhost/pico-launcher-medium9-font:1 python /workspace/tools/generate_medium9_font.py --output /output/NotoSansJP-Medium-9.nft2 --cache /font-cache/NotoSansJP-Medium.otf`
- Deterministic double generation produced byte-identical outputs: 18,809 bytes, SHA-256 `d3699e00e6a26fb250a415307418cee6a1bb77caaeb666797fcf0a7bdd21d278`.
- Measured NFT2: signature `NFT2`; glyph table `0x14`; charmap `0x0D34`; glyph data `0x115C`; 420 glyphs; 4-bit coverage; ascend/descend `11/3`; line height 14; representative ink y `4..13` (date) and `4..11` (HIGH/LOW/nickname). Widths: `12/31 23:59` 49/64, `HIGH` 23/24, `LOW` 20/24, `WWWW ... W` 53/66. All geometry contracts fit; `[190,256)`, the 16px strip, coverage-capacity derivation, bounded dirty-row publication/cache flush, icons, and colors remain unchanged.
- Added OFL-1.1 text at `arm9/data/OFL-1.1.txt` and asset-specific provenance at `arm9/data/NotoSansJP-Medium-9.PROVENANCE.md`; the font asset is not relabeled under `LICENSE.txt`.
- Status-only wiring selects `FontType::Medium9` for center date/time, NTR fallback, nickname, compositor, and glyph lookup. Generic Medium10 callers remain unchanged. `main` unpacks the generated asset and the generic `.nft2` bin2c rule packages it.
- `3.3` remains unchecked and the physical matrix remains pending/unaccepted. No SD access, installation, hardware claim, commit, push, PR, or archive occurred.

## TDD Cycle Evidence — medium9-status-font
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| Reproducible generated asset | `tests/test_medium9_font.py` | Unit | N/A (new) | Missing generator import failed, exit 1 | 1 test passed, exit 0 | NFT2 layout, height, and four width bounds | Minimal stdlib validator plus pinned renderer only |
| Status font wiring and metrics | `tests/status_bar_format_tests.cpp` | Unit/source contract | Existing focused status test passed before edits | Medium10 metrics/width static assertions failed, exit 1 | Passed, exit 0 | Center/NTR/nickname bounds and repository/main/view wiring | Reused existing generic bin2c path |

## Work Unit Evidence — medium9-status-font
| Evidence | Exact result |
|---|---|
| Focused C++ | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests` — exit 0; 2 executables passed. |
| Coverage-capacity C++ | `sed '1s@#include "common.h"@#include <nds/ndstypes.h>@' arm9/source/gui/views/Nft2CoverageRenderer.cpp > /tmp/pico-launcher-Nft2CoverageRenderer-host.cpp && g++ -std=c++23 -Wall -Wextra -Wpedantic -I/tmp/pico-launcher-host-stubs -Iarm9/source -Iarm9/source/gui/views tests/nft2_coverage_renderer_tests.cpp /tmp/pico-launcher-Nft2CoverageRenderer-host.cpp -o /tmp/pico-launcher-nft2-coverage-tests && /tmp/pico-launcher-nft2-coverage-tests` — exit 0; verified 64/24/66 render paths and `NicknameTextWidth * 16` canaries. |
| Targeted Python | `python3 -m unittest tests/test_medium9_font.py tests/test_status_bar_assets.py` — exit 0; 5 tests passed. |
| Generator/asset validation | `podman run ... generate_medium9_font.py --validate --output /output/NotoSansJP-Medium-9.nft2` — exit 0; pinned source hash, canonical charmap, NFT2 offsets/count, height, and widths reported above. |
| Runtime harness | `podman run --rm --entrypoint /bin/sh -v /tmp/pico-launcher-medium9-status-font:/workspace:Z -w /workspace docker.io/skylyrac/blocksds@sha256:7bea1d0b6026b27036e16ab089770e9864f5844848ccf51e1882d278be116650 -c 'make clean && make -j1'`, then `make -j1` after the initial command timeout — exit 0; clean serial BlocksDS v1.16.0 build. |
| ROM | `/tmp/pico-launcher-medium9-status-font/LAUNCHER.nds`, 542,208 bytes, SHA-256 `b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`. |
| Process/diff | `git diff --check` — exit 0 after generation and build. |
| Rollback boundary | Revert Medium9 asset/provenance/license, generator container inputs, the five font-wiring source files, and Medium9 test assertions; generic font packaging and unrelated status composition remain intact. |

## Result Contract — medium9-status-font attempt 2
```yaml
schema: gentle-ai.medium9-status-font-result/v1
evidence_revision: sha256:625c2746744e7d739828867e49386927603bef23705a56df2036a3952074f609
settlement_evidence_revision: sha256:1664844142cdd5347363c413c1996b3123bbec0871b72282bb61ba4acd50062a
change: compose-status-text-on-theme-background
work_unit: medium9-status-font
result: success_pending_physical_matrix
native_attempt_token: sha256:ac561170a782ac96fa69333e67498a34f4178b2f17fd5c9d1f4ae1456429138f
native_attempt_acquired: false
native_attempt_settled: false
font_asset: arm9/data/NotoSansJP-Medium-9.nft2
font_asset_sha256: d3699e00e6a26fb250a415307418cee6a1bb77caaeb666797fcf0a7bdd21d278
rom: /tmp/pico-launcher-medium9-status-font/LAUNCHER.nds
rom_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
physical_matrix: pending_unaccepted
task_3_3_completed: false
```


## Launcher-Only DSteam Installation Retry — Status Horizontal Alignment
- Native token `sha256:bb6f11174c1c955cf5bec0b348f097b71464d109dee99b0d18959c97706db674` was neither acquired nor settled.
- UUID-directed identity passed independently: `/dev/disk/by-uuid/0BFC-CD24` resolved to `/dev/sda1`; `findmnt`, udev, UDisks, and `/sys/class/block/sda1/ro` agreed on a mounted rw `vfat` filesystem at `/run/media/guill3/0BFC-CD24`.
- Source identity passed before and after preflight without rebuilding: `/tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds`, 542208 bytes, `sha256:c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`.
- Predecessor identity passed twice: `/_picoboot.nds`, 542208 bytes, `sha256:b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`.
- The complete 53-entry root manifest was readable. Under the canonical sorted compact-JSON root-manifest method used by the prior installer, its observed digest was `sha256:7209a710c336f932f39b0923b962282945e4e9b42b8ebf035ada46d47359ab93`, which does not equal the supplied expected `sha256:1ce21d6c5418a53c61cef2c90c9ec0f992180d613eb29102efd413a0c64768a4`.
- **Fail-closed result: no media mutation.** No backup, stage, atomic replacement, fsync/syncfs/sync, installed-file readback, post-state root diff, or unmount was attempted. No visual, no-tearing, or physical-acceptance claim is made; task 3.3 remains unchecked and the physical matrix remains pending/unaccepted.

## Installation Work Unit Evidence — Status Horizontal Alignment Retry
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `stat -c 'path=%n size=%s' /tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds && sha256sum /tmp/pico-launcher-status-horizontal-alignment/LAUNCHER.nds` — exit 0 before and after preflight; 542208 bytes and `c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9`. Target predecessor recheck also exited 0 at 542208 bytes and `b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5`. |
| Runtime harness command/scenario and exact result | UUID-directed, launcher-only preflight — blocked before mutation because the independently captured complete root-manifest digest did not match the supplied expected digest. |
| Rollback boundary | N/A: no media mutation occurred; `/_picoboot.nds` and every root entry remain unchanged by this executor. |

## Result Contract — Status Horizontal Alignment Installation Retry
```yaml
schema: gentle-ai.installation-result/v1
change: compose-status-text-on-theme-background
work_unit: status-horizontal-alignment-install-retry
result: blocked_pre_mutation_manifest_digest_mismatch
native_attempt_token: sha256:bb6f11174c1c955cf5bec0b348f097b71464d109dee99b0d18959c97706db674
native_attempt_acquired: false
native_attempt_settled: false
media_uuid: 0BFC-CD24
media_device: /dev/sda1
media_mountpoint: /run/media/guill3/0BFC-CD24
media_filesystem: vfat
target: /_picoboot.nds
candidate_bytes: 542208
candidate_sha256: c2ab4530930b7cde27ef90b5d6f08bdcf2695ae83d89a79b92dfdb84cf991cb9
predecessor_bytes: 542208
predecessor_sha256: b82eff4d27af2accec10c8dcbf052b03c2bd96b41ce8f9c35aead4d36a2e36b5
root_pre_manifest_expected_sha256: 1ce21d6c5418a53c61cef2c90c9ec0f992180d613eb29102efd413a0c64768a4
root_pre_manifest_observed_sha256: 7209a710c336f932f39b0923b962282945e4e9b42b8ebf035ada46d47359ab93
root_pre_manifest_entries: 53
manifest_readability: complete
backup_created: false
target_replaced: false
durability: not_started
readback: not_started
root_diff: not_proven_no_mutation
temporary_artifact_absent: true
unmount_attempted: false
unmount_proven: false
source_preserved: true
physical_matrix: pending_unaccepted
task_3_3_completed: false
no_visual_claim: true
no_tearing_claim: true
evidence_revision: sha256:9abe18a51eb0b49e6cb9b2d08e71fd287bd177ca9263aba899d10718fb47fc3b
```
```json
{"change":"compose-status-text-on-theme-background","mutation_performed":false,"native_attempt_acquired":false,"native_attempt_settled":false,"native_attempt_token":"sha256:bb6f11174c1c955cf5bec0b348f097b71464d109dee99b0d18959c97706db674","reason":"Pre-mutation root-manifest digest mismatch; fail closed before backup, staging, replacement, durability, readback, root-diff, or unmount.","schema":"gentle-ai.installation-settlement-evidence/v1","settlement_evidence_revision":"sha256:c6530f87f3e2b9ed9d5608c3551cfb7f169778c037c129d8098ec01679715bf5","work_unit":"status-horizontal-alignment-install-retry"}
```

## Limited Physical Acceptance Reconciliation
- The maintainer selected an intentional limited-scope close: only the final observed hardware configuration is accepted. The original 5×4/20-cell matrix is **NOT EXECUTED** and **NOT PASSED**.
- Final geometry uses `StatusNicknameRight=256` and `StatusBatteryGap=18`, preserving all battery coordinates of the physically accepted 4px-left candidate; DSi/NTR clamps prevent overlap, so the configured gap does not assert every clamped effective gap. Bounds: DSi short `{212,4,228,12}`, empty `{222,4,238,12}`, maximum `{160,4,176,12}`; NTR short `{212,4,228,12}`, maximum `{168,4,184,12}`.
- Fresh final-candidate checks passed: geometry executable, NFT2 renderer executable, status-strip compositor executable, Python Medium9/status assets 5/5, and `git diff --check`.
- Clean serial BlocksDS v1.16.0 build passed: `/tmp/pico-launcher-nickname-right-two/LAUNCHER.nds`, 542208 bytes, SHA-256 `23699aefafdd6d0b8691e8b9017898e2d431b1e517ac5334768c027ba9b5c73e`.
- That ROM was installed on DSteam UUID `0BFC-CD24`; predecessor backup `/_picoboot.nds.backup-pre-nickname-right-two-20260814T200501201214Z` is SHA-256 `90ddaa26f03afb4675ab2215f174ccbbebbca011a3bfd839faec6373abaff3a9`. Pre/post manifests were 458/433 and 459/434 paths/files; semantic diff was target replacement plus one backup, durability readback and final unmount passed.
- The user physically accepted the final observed layout as perfect. No theme/mode/minute/NTR coverage beyond automated contracts and this observed configuration is claimed; archive must identify the intentional limited-scope close.

## TDD Cycle Evidence — limited-physical-acceptance-reconcile
| Task | RED | GREEN | REFACTOR |
|---|---|---|---|
| 3.3 limited acceptance record | N/A — artifact-only scope reconciliation; no production behavior changed | N/A — no production behavior changed | N/A — no production code refactor |
| Four legacy matrix limitations | N/A — converted stale unchecked entries into explicit non-execution evidence | N/A — no production behavior changed | N/A — documentation-only reconciliation |

## Work Unit Evidence — limited-physical-acceptance-reconcile
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `git diff --check` — exit 0. The recorded final candidate also passed the geometry, NFT2 renderer, status-strip compositor, and Python Medium9/status-assets checks (5/5). |
| Runtime harness command/scenario and exact result | N/A — this unit changes only SDD artifacts and does not access hardware, build, install, or alter production behavior; verify must independently rerun runtime checks. |
| Rollback boundary | Revert only `proposal.md`, delta `spec.md`, `design.md`, `tasks.md`, and this reconciliation section; product source, tests, assets, ROM, and DSteam media remain untouched. |

## Gatekeeper Artifact Reconciliation
- Documentation-only correction: final implementation state is Medium9 with bounded dirty-row publication and an ARM9 DMA source cache flush.
- The final observed configuration remains accepted; the original 5×4/20-cell matrix is **NOT EXECUTED** and **NOT PASSED**.

## TDD Cycle Evidence — gatekeeper-artifact-reconciliation
| Task | RED | GREEN | REFACTOR |
|---|---|---|---|
| Gatekeeper artifact reconciliation | N/A — no production behavior changed | N/A — structural readback only | N/A — documentation-only correction |

## Work Unit Evidence — gatekeeper-artifact-reconciliation
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | Structural readback confirmed 19 checked tasks, final Medium9/dirty-row/geometry wording, no unsupported exact-64-line claim, and `git diff --check` — exit 0. |
| Runtime harness command/scenario and exact result | N/A — artifact-only correction; no runtime boundary, build, hardware, or media access. |
| Rollback boundary | Revert only the final-state wording in `design.md`, `tasks.md`, and this section; no product source, tests, assets, ROM, or DSteam media is affected. |

## Native-Bounded Remediation — final-limited-scope-verification
- Native status requires remediation for `sha256:8dbfe28155777f72844ecf70d4f08bb409ab7a8eea5f41c9042fa9daf0e2d506`. Its unmanaged binding has `lineage_id: ""`, `generation: 0`, and `fix_batch: 0`; no review lineage was invented.
- The active parent-owned token `sha256:b4ea3fd0e8df94aa9812fe5a80e7c50aad6b02a9a454aca28466f8a254ebb176` was not acquired or settled by this executor.
- `RefreshTextMode()` now gates both composed and material presentations on `_statusGraphicsReady`. It restores the pristine custom strip after any incomplete field composition, then clears direct palettes for every binary fallback.
- This remediation changed 47 authored lines: 34 additions and 13 deletions across the three files below. It preserves Medium9, nickname x=256, configured gap 18, battery geometry, icon behavior when graphics are ready, and VBlank publication.

### TDD Cycle Evidence — final-limited-scope-verification
| Task | Test File | Layer | Safety Net | RED | GREEN | TRIANGULATE | REFACTOR |
|---|---|---|---|---|---|---|---|
| final-limited-scope-verification | `tests/status_bar_format_tests.cpp` | Unit plus source wiring contract | ✅ Geometry, NFT2 renderer, compositor, Python 5/5, and `git diff --check` all passed before edits | ✅ Test changed first; geometry compile exited 1 because the three-argument mode resolver and pristine-restore helper did not exist | ✅ Geometry test passed after the smallest shared helper and view guard; final focused checks and build passed | ✅ Executable cases cover ready custom/material/binary, unavailable custom/material fallback, and custom partial/success/unsupported restore decisions | ➖ None needed |

### Work Unit Evidence — final-limited-scope-verification
| Evidence | Exact result |
|---|---|
| Focused test command and exact result | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/pico-launcher-status-bar-format-tests && /tmp/pico-launcher-status-bar-format-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -I/tmp/opencode/pico-launcher-final-verify-20260814/host-stubs -Iarm9/source -Iarm9/source/gui/views tests/nft2_coverage_renderer_tests.cpp /tmp/opencode/pico-launcher-final-verify-20260814/Nft2CoverageRenderer-host.cpp -o /tmp/pico-launcher-nft2-coverage-renderer-tests && /tmp/pico-launcher-nft2-coverage-renderer-tests && g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/pico-launcher-status-strip-compositor-tests && /tmp/pico-launcher-status-strip-compositor-tests && PYTHONDONTWRITEBYTECODE=1 python3 -m unittest tests/test_medium9_font.py tests/test_status_bar_assets.py && git diff --check` — exit 0; geometry, NFT2 64/24/66 canaries, and compositor executables passed; Python 5/5 passed; whitespace check passed. |
| Runtime harness command/scenario and exact result | `podman run --rm --network=none --security-opt label=disable --entrypoint /bin/sh --mount type=bind,src=/home/guill3/Documents/Hobbies/dspico/pico-launcher,dst=/source,ro=true --mount type=bind,src=/tmp/opencode/pico-launcher-vf001-remediation/build,dst=/workspace -w /workspace docker.io/skylyrac/blocksds@sha256:7bea1d0b6026b27036e16ab089770e9864f5844848ccf51e1882d278be116650 -c 'set -eu; cp -a /source/. /workspace/; make clean; make -j1; sha256sum LAUNCHER.nds; stat -c "%s %n" LAUNCHER.nds'` — exit 0; one clean serial BlocksDS v1.16.0 build produced `LAUNCHER.nds`, 542208 bytes, `sha256:54dd7628b76a03df312a54125ebc627b5fdc448eb292d32016adc1b8180da585`. Existing libtwl/libnds register-macro redefinition warnings remained non-fatal. No SD, hardware, install, review, archive, acquire, or settle action occurred. |
| Rollback boundary | Revert only this unit's resolver/restore changes in `arm9/source/romBrowser/views/StatusBarFormat.h`, the guarded fallback flow in `arm9/source/romBrowser/views/RomBrowserTopScreenView.cpp`, and the new assertions in `tests/status_bar_format_tests.cpp`; unrelated dirty/untracked work, background composition, geometry, assets, and artifacts remain intact. |

### Native Settlement Preimage
The following UTF-8 text, including its final newline, hashes to `sha256:419c62882cd3b56b1a61d1a69b5bccd38f7b05f42b418466877430e4d2d98671`:

```text
schema=gentle-ai.remediation-evidence/v1
change=compose-status-text-on-theme-background
artifact_store=hybrid
mode=strict_tdd
authority_token=sha256:b4ea3fd0e8df94aa9812fe5a80e7c50aad6b02a9a454aca28466f8a254ebb176
work_unit=final-limited-scope-verification
evidence_goal=prove-runtime-and-scoped-acceptance
failed_evidence_revision=sha256:8dbfe28155777f72844ecf70d4f08bb409ab7a8eea5f41c9042fa9daf0e2d506
remediation_mode=unmanaged
lineage_id=
generation=0
fix_batch=0
head=36a3d30b501de9bf3775a6b1898a377bd3bed172
tree=72b94ec32384e8cb65ad668c325d64670080bfb3
candidate_worktree_diff_sha256=sha256:c6c872e30613de3fb6c4c1ff042efb5cf4e819c6bdaf9ae52159050a8834a435
changed_lines=47
status_geometry=pass
nft2_coverage=pass
status_strip_compositor=pass
targeted_python=pass_5_of_5
git_diff_check=pass
blocksds_image=docker.io/skylyrac/blocksds@sha256:7bea1d0b6026b27036e16ab089770e9864f5844848ccf51e1882d278be116650
blocksds=pass
rom_path=/tmp/opencode/pico-launcher-vf001-remediation/build/LAUNCHER.nds
rom_bytes=542208
rom_sha256=54dd7628b76a03df312a54125ebc627b5fdc448eb292d32016adc1b8180da585
hardware=not_run
sd=not_accessed
```

### Result Contract — final-limited-scope-verification
```yaml
schema: gentle-ai.remediation-result/v1
evidence_revision: sha256:419c62882cd3b56b1a61d1a69b5bccd38f7b05f42b418466877430e4d2d98671
failed_evidence_revision: sha256:8dbfe28155777f72844ecf70d4f08bb409ab7a8eea5f41c9042fa9daf0e2d506
lineage_id: ""
generation: 0
fix_batch: 0
remediation_mode: unmanaged
change: compose-status-text-on-theme-background
work_unit: final-limited-scope-verification
evidence_goal: prove-runtime-and-scoped-acceptance
status: success
executive_summary: Status-graphics unavailability now selects truthful binary fallback, and incomplete custom composition restores pristine pixels before fallback.
artifacts:
  - arm9/source/romBrowser/views/StatusBarFormat.h
  - arm9/source/romBrowser/views/RomBrowserTopScreenView.cpp
  - tests/status_bar_format_tests.cpp
  - openspec/changes/compose-status-text-on-theme-background/apply-progress.md
  - sdd/compose-status-text-on-theme-background/apply-progress
next_recommended: sdd-verify
risks: Limited physical acceptance remains contextual; no hardware or SD validation was performed.
skill_resolution: paths-injected
native_attempt_token: sha256:b4ea3fd0e8df94aa9812fe5a80e7c50aad6b02a9a454aca28466f8a254ebb176
native_attempt_acquired: true
native_attempt_acquired_by_executor: false
native_attempt_settled: false
changed_lines: 47
```
```json
{"schema":"gentle-ai.remediation-evidence/v1","evidence_revision":"sha256:419c62882cd3b56b1a61d1a69b5bccd38f7b05f42b418466877430e4d2d98671","evidence_preimage_sha256":"sha256:419c62882cd3b56b1a61d1a69b5bccd38f7b05f42b418466877430e4d2d98671","evidence_preimage_final_newline":true,"failed_evidence_revision":"sha256:8dbfe28155777f72844ecf70d4f08bb409ab7a8eea5f41c9042fa9daf0e2d506","lineage_id":"","generation":0,"fix_batch":0,"remediation_mode":"unmanaged","change":"compose-status-text-on-theme-background","work_unit":"final-limited-scope-verification","evidence_goal":"prove-runtime-and-scoped-acceptance","native_attempt_token":"sha256:b4ea3fd0e8df94aa9812fe5a80e7c50aad6b02a9a454aca28466f8a254ebb176","native_attempt_acquired":true,"native_attempt_acquired_by_executor":false,"native_attempt_settled":false,"changed_lines":47,"focused_checks":{"status_geometry":"pass","nft2_coverage":"pass: 64/24/66 canaries","status_strip_compositor":"pass","targeted_python":"pass: 5/5","git_diff_check":"pass"},"runtime":{"blocksds":"pass: BlocksDS v1.16.0","rom":{"path":"/tmp/opencode/pico-launcher-vf001-remediation/build/LAUNCHER.nds","bytes":542208,"sha256":"54dd7628b76a03df312a54125ebc627b5fdc448eb292d32016adc1b8180da585"}},"hardware":"not_run","sd":"not_accessed","rollback_boundary":"Revert only StatusBarFormat.h, RomBrowserTopScreenView.cpp, and status_bar_format_tests.cpp changes from this remediation."}
```
