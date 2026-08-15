```yaml
schema: gentle-ai.verify-result/v1
evidence_revision: sha256:fe4045cdca9ab885f0f6e0430e1c1f21a7482ebbca95e4fb377af917e6844e13
verdict: pass_with_warnings
blockers: 0
critical_findings: 0
requirements: 5/5
scenarios: 8/8
test_command: (g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_bar_format_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_bar_format_tests) && (g++ -std=c++23 -Wall -Wextra -Wpedantic -I/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/host-stubs -Iarm9/source -Iarm9/source/gui/views tests/nft2_coverage_renderer_tests.cpp /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/Nft2CoverageRenderer-host.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/nft2_coverage_renderer_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/nft2_coverage_renderer_tests) && (g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_strip_compositor_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_strip_compositor_tests) && (PYTHONDONTWRITEBYTECODE=1 python3 -m unittest tests/test_medium9_font.py tests/test_status_bar_assets.py) && (git diff --check)
test_exit_code: 0
test_output_hash: sha256:ec20e26c577e82f1094098c10070fc14808da267800355819a86b29c92ed0f5a
build_command: podman run --rm --network=none --security-opt label=disable --entrypoint /bin/sh --mount type=bind,src=/home/guill3/Documents/Hobbies/dspico/pico-launcher,dst=/source,ro=true --mount type=bind,src=/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/build,dst=/workspace -w /workspace docker.io/skylyrac/blocksds@sha256:7bea1d0b6026b27036e16ab089770e9864f5844848ccf51e1882d278be116650 -c 'set -eu; cp -a /source/. /workspace/; make clean; make -j1; sha256sum LAUNCHER.nds; stat -c "%s %n" LAUNCHER.nds'
build_exit_code: 0
build_output_hash: sha256:4a4cd8198ba08a6fcb5f941d7355dadccd193c134e4a271c4ebebfa56d7f4882
```

## Verification Report

**Change**: `compose-status-text-on-theme-background`
**Version**: N/A
**Mode**: Strict TDD
**Artifact store**: Hybrid (OpenSpec + Engram)
**Native authority**: orchestrator-owned active revision/token `sha256:4f4a676d76f717a450c4585fd23153203c6cb4b30acb22d4bf03a27178450964`
**Work unit**: `post-vf001-final-verification`
**Evidence goal**: `prove-remediated-runtime-and-scoped-acceptance`
**Prior failed verification**: `sha256:8dbfe28155777f72844ecf70d4f08bb409ab7a8eea5f41c9042fa9daf0e2d506`
**Bound remediation evidence**: `sha256:419c62882cd3b56b1a61d1a69b5bccd38f7b05f42b418466877430e4d2d98671`

The active preterminal runtime record and the preceding remediation-finish record were read from the Git-common-dir ledger. Their exact JSON preimages hash to their record revisions (`4f4a...` and `ab273...`), and the remediation record binds VF-001 to the prior failed evidence. This verifier did not acquire, settle, reset, rescope, review, remediate, or archive.

### Completeness

| Metric | Value |
|---|---:|
| Requirements total | 5 |
| Requirements compliant | 5 |
| Scenarios total | 8 |
| Scenarios compliant | 8 |
| Tasks total | 19 |
| Tasks complete | 19 |
| Tasks incomplete | 0 |

The current delta spec has four modified requirements and one removed requirement entry, for five requirements total. It has eight scenarios. All 19 task checkboxes are complete, so full verification was permitted.

### Build & Tests Execution

| Check | Exact command | Exit | Exact-output SHA-256 | Result |
|---|---|---:|---|---|
| Status geometry and VF-001 truth table | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_bar_format_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_bar_format_tests` | 0 | `sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | PASS; includes ready/unavailable custom/material mode cases and incomplete-composition restore decisions |
| NFT2 bounded coverage | `g++ -std=c++23 -Wall -Wextra -Wpedantic -I/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/host-stubs -Iarm9/source -Iarm9/source/gui/views tests/nft2_coverage_renderer_tests.cpp /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/Nft2CoverageRenderer-host.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/nft2_coverage_renderer_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/nft2_coverage_renderer_tests` | 0 | `sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | PASS; 64/24/66px paths and canaries |
| Status-strip compositor | `g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_strip_compositor_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_strip_compositor_tests` | 0 | `sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | PASS; blend, clipping, restoration, backend guards, and packed tile bytes |
| Targeted Python | `PYTHONDONTWRITEBYTECODE=1 python3 -m unittest tests/test_medium9_font.py tests/test_status_bar_assets.py` | 0 | `sha256:ec20e26c577e82f1094098c10070fc14808da267800355819a86b29c92ed0f5a` | PASS; 5/5 |
| Whitespace | `git diff --check` | 0 | `sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | PASS; no output |
| Combined test evidence | YAML `test_command` above | 0 | `sha256:ec20e26c577e82f1094098c10070fc14808da267800355819a86b29c92ed0f5a` | PASS |
| Isolated BlocksDS build | YAML `build_command` above | 0 | `sha256:4a4cd8198ba08a6fcb5f941d7355dadccd193c134e4a271c4ebebfa56d7f4882` | PASS; one clean serial, network-disabled build |

Exact targeted Python output:

```text
.....
----------------------------------------------------------------------
Ran 5 tests in 0.363s

OK
```

The NFT2 host shim is isolated under `/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814`; its typedef header hashes to `sha256:479248988c8954c0b39c2a6802d6aba4cfdcdb0d546c765bf9ee4573d6a5c97a`, and the translation of the current renderer hashes to `sha256:930bc294090e6fbb8536db1f9dd2a11087a011df4e8138d4e89b13006deb8627`.

The build used the exact pinned image reference, `--network=none`, a read-only source mount, a separate isolated output mount, `make clean`, and `make -j1`. The local image ID was `sha256:bbc583158bf188b895974462ad7a950226406e96004b385ce4ead156327abcc5`; build output identified NDSTOOL v1.16.0.

| ROM evidence | Bytes | SHA-256 | Role |
|---|---:|---|---|
| `/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/build/LAUNCHER.nds` | 542208 | `54dd7628b76a03df312a54125ebc627b5fdc448eb292d32016adc1b8180da585` | Fresh post-VF-001 independent build; identical to the remediation build identity |
| `/tmp/pico-launcher-nickname-right-two/LAUNCHER.nds` | 542208 | `23699aefafdd6d0b8691e8b9017898e2d431b1e517ac5334768c027ba9b5c73e` | Previously installed and accepted final observed configuration; contextual only |

No SD or hardware access occurred. The original 20-cell matrix remains **NOT EXECUTED** and **NOT PASSED**.

**Coverage**: Skipped — no coverage tool is configured.
**Linter**: Not available.
**Type checker**: Not available.

### Spec Compliance Matrix

| Requirement | Scenario | Passing executable evidence | Result |
|---|---|---|---|
| Fixed Opaque Rail | Theme pixels survive composition | Compositor executable passed coverage 0/1/8/15, bit-15 preservation, clipping, pristine restoration, and canaries; asset tests prove the rail/profile assets are absent | ✅ COMPLIANT |
| Fixed Opaque Rail | Unsupported representation | Backend mode truth tables and the status-mode executable prove unsupported input selects binary fallback without guessed backing | ✅ COMPLIANT |
| Profile Treatment and Collision-Free Composition | Coverage and nickname bounds | Geometry executable proves nickname `[190,256)`, Medium9 metrics, configured gap 18, preserved/clamped battery coordinates; NFT2 executable proves 64/24/66px canaries | ✅ COMPLIANT |
| Profile Treatment and Collision-Free Composition | Nickname shortening | Geometry executable proves unsupported-glyph substitution, ellipsis limits/width, right edge x=256, and fixed neighboring bounds | ✅ COMPLIANT |
| Profile Treatment and Collision-Free Composition | Readable centered status text | Medium9 Python validation and geometry executable prove baseline 1, measured widths, and ink within y=0..15 | ✅ COMPLIANT |
| Graphics-Allocation Truthful Fallback | Text changes recompose cleanly | Status executable covers unavailable custom/material fallback and incomplete/successful composition restore decisions; compositor restoration passes; inspected view code restores before composition and again after any incomplete field set | ✅ COMPLIANT |
| Graphics-Allocation Truthful Fallback | Material guard and failure | Compositor executable proves uniform/nonuniform tile guard, exact endpoint, packed byte view, and unsupported fail-closed mode | ✅ COMPLIANT |
| Preserved Pipeline and Release Evidence | Host and limited physical acceptance | All fresh host checks and the clean pinned build passed; retained contextual evidence records only the maintainer-accepted final observed covers-active configuration | ✅ COMPLIANT |

**Compliance summary**: 8/8 scenarios compliant. The limited physical acceptance does not convert any unexecuted matrix cell into passing evidence.

### Correctness (Static Evidence)

| Requirement | Status | Notes |
|---|---|---|
| Fixed Opaque Rail | ✅ Implemented | Custom composition restores pristine pixels and changes only covered strip pixels; fixed rail/profile source and assets are absent. |
| Profile Treatment and Collision-Free Composition | ✅ Implemented | Medium9, full NFT2 coverage, nickname `[190,256)`, x=256, configured gap 18, and preserved DSi/NTR clamped battery coordinates remain present. |
| Graphics-Allocation Truthful Fallback | ✅ Implemented | `StatusTextMode` forces binary fallback when graphics are unavailable. `RefreshTextMode` gates composed/material paths on `_statusGraphicsReady`, restores pristine before custom composition, and restores again when any field fails. |
| Preserved Pipeline and Release Evidence | ✅ Implemented within accepted scope | App VBlank calls the background before the view; custom publication uses bounded dirty rows with ARM9 source-cache flush and DMA waits; all three reconstruction sites inject the background. |
| Removed Fixed Rail and Profile Placeholder | ✅ Removed | The obsolete behavior is absent and the delta records migration to theme-owned composition. |

### Coherence (Design)

| Decision | Followed? | Notes |
|---|---|---|
| Background-owned composition contract | ✅ Yes | `IThemeBackground` owns synchronous prepare, restore, mode, and endpoint contracts. |
| Restore-before-blend RGB555 compositor | ✅ Yes | The compositor restores pristine, clips half-open bounds, preserves bit 15, and writes only nonzero coverage. |
| Guarded material endpoint | ✅ Yes | Material remains tiled and uses only the executable uniform-index-16 invariant and exact RGB555 endpoint. |
| Bounded VBlank publication | ✅ Yes | Dirty rows are budget-checked, cache-flushed, DMA-copied, and waited before view/cover VBlank work. |
| Medium9 readability geometry | ✅ Yes | Status text uses Medium9, nickname right edge x=256, configured gap 18, and the reconciled DSi/NTR clamps. |
| Truthful atomic failure fallback | ✅ Yes | Unavailable graphics bypass composed/material selection; incomplete custom composition restores pristine before binary fallback and clears direct palettes. |

### TDD Compliance

| Check | Result | Details |
|---|---|---|
| TDD evidence reported | ✅ | `apply-progress.md` contains base RED/GREEN evidence and a dedicated VF-001 remediation table. |
| All code-bearing work has tests | ✅ | Five current test files cover geometry/state/source wiring, compositor/backends, NFT2 bounds, Medium9, and status assets. |
| RED confirmed | ✅ | The VF-001 row records test-first compile failure; every named test file exists. |
| GREEN confirmed now | ✅ | Three focused C++ executables and 5/5 Python tests passed independently in this verification. |
| Triangulation adequate | ✅ | VF-001 cases vary ready/unavailable custom/material/binary modes and incomplete/successful/non-composed restore decisions. |
| Safety net for modified files | ⚠️ | The VF-001 remediation has an explicit pre-edit safety net; the historical base 1.1–3.2 table lacks a per-row safety-net column. |

**TDD compliance**: 5/6 checks fully passed; the remaining documentation-shape gap is historical and does not invalidate current executable evidence.

### Test Layer Distribution

| Layer | Tests | Files | Tools |
|---|---:|---:|---|
| Unit/host contract | 8 top-level entry points | 5 | C++23 `assert`/`static_assert`, Python `unittest` |
| Integration | 0 | 0 | Not configured |
| E2E | 0 | 0 | Not configured |
| **Total** | **8** | **5** | |

### Changed File Coverage

Coverage analysis skipped — no coverage tool is configured.

### Assertion Quality

**Assertion quality**: ✅ No tautology, orphan empty assertion, type-only-only test, assertion without production behavior/source contract, ghost loop, smoke-only assertion, implementation-class assertion, or mock-heavy test was found. Fixed loops have non-empty domains and varied expected values; asset mutation tests prove rejection behavior.

### Quality Metrics

**Linter**: ➖ Not available
**Type checker**: ➖ Not available
**Focused compiler diagnostics**: ✅ `-Wall -Wextra -Wpedantic` emitted no diagnostics
**Target compiler diagnostics**: ⚠️ Build passed with existing libtwl/libnds register-macro redefinition warnings
**Whitespace validation**: ✅ `git diff --check` passed

### Issues Found

**CRITICAL**: None.

**WARNING**:

1. Physical acceptance is intentionally limited to the final observed covers-active configuration. The original 5×4/20-cell matrix was not executed and must never be represented as passed.
2. The fresh post-VF-001 ROM exactly matches the remediation build (`54dd...`) but differs from the previously installed/accepted ROM (`23699...`). With no SD/hardware access, physical acceptance remains contextual rather than binary-equivalence evidence for this rebuilt candidate.
3. Historical base TDD rows do not include the strict module's per-row safety-net field, although the VF-001 remediation row does.
4. The target build retains pre-existing libtwl/libnds register-macro redefinition warnings; they are non-fatal and not introduced by VF-001.

**SUGGESTION**:

1. A future host fake-background harness could assert exact restore call ordering at the view boundary; current executable decision cases plus inspected wiring are sufficient for the reconciled scenario.

### Verdict

**PASS WITH WARNINGS**

VF-001 is remediated: unavailable status graphics force truthful binary fallback, late custom-composition failure restores pristine pixels before fallback, and fresh executable/build evidence preserves successful composed/material behavior and reconciled geometry. There are zero critical findings and zero archive blockers in this report. Native settlement remains orchestrator-owned.

### Exact Settlement Evidence

The following exact UTF-8 preimage, including its final newline, hashes to `sha256:fe4045cdca9ab885f0f6e0430e1c1f21a7482ebbca95e4fb377af917e6844e13`. The orchestrator must preserve these exact bytes for native settlement and the later GateRequest:

```text
schema=gentle-ai.verification-evidence/v1
change=compose-status-text-on-theme-background
artifact_store=hybrid
mode=strict_tdd
authority_revision=sha256:4f4a676d76f717a450c4585fd23153203c6cb4b30acb22d4bf03a27178450964
authority_token=sha256:4f4a676d76f717a450c4585fd23153203c6cb4b30acb22d4bf03a27178450964
authority_record_preimage_sha256=sha256:4f4a676d76f717a450c4585fd23153203c6cb4b30acb22d4bf03a27178450964
remediation_finish_record_preimage_sha256=sha256:ab273e3edecd66a3f441c9ace1e9ca64aaa6e690f017b3068faff8ac3031e6ca
work_unit=post-vf001-final-verification
evidence_goal=prove-remediated-runtime-and-scoped-acceptance
runtime_objective_id=sha256:c96d823f3566cf7fc15dbf73c9de115bb782088aeaa9f0ebbccb0f62a7cf16bd
runtime_objective_generation=30
begin_candidate_identity=sha256:2cb69a540d6dacf1844ae5c0855977ca419ce13205f1ba9acd8bb8a2b5032a55
begin_candidate_tree=628c212e922ab1e5411bc6a45d341e32fc7bfd3f
head=36a3d30b501de9bf3775a6b1898a377bd3bed172
prior_failed_verification=sha256:8dbfe28155777f72844ecf70d4f08bb409ab7a8eea5f41c9042fa9daf0e2d506
bound_remediation_evidence=sha256:419c62882cd3b56b1a61d1a69b5bccd38f7b05f42b418466877430e4d2d98671
prior_verify_report_sha256=e6901b76670c325e0fe820c9df03ed2036dd963be04a06a2ef8fc4f9ed4c5cb8
proposal_sha256=0fc7126788175c33266f16c8b4065a54f00c33003dabf9bc07c74f96c8a4b2bd
spec_sha256=4f4430c078800c736d80cb446ba74586dfdfe93a8830baeb76f174efe61ad92b
design_sha256=d6ada30765f387b39ae1449bc314f557c4b457f47bebd64d865bb63727958d85
tasks_sha256=0c94d6c611ea52d75781a9913e54bad6448a4d385ce3c373bcb9f1f9d29bc315
apply_progress_sha256=7be0bb5cb44b61dbc4076e67c3a55b56669421fbd46182558f8f256ba5b19b56
status_bar_format_sha256=aaa0a96a1bd7f10d41d6c71a6a4047a6d90f1878933f8f1f64eb1e958b59aa21
rom_browser_top_screen_view_sha256=0931b367072583f1a08ad99ca5cd806fc81fcdd91529f2a64d68d4b62c28b13f
status_bar_format_tests_sha256=35ccca2d70281c668da435debf14e379f3b018b6b87dcb393addc07a8d8c2236
tasks=19/19
requirements=5/5
scenarios=8/8
test_command=(g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_bar_format_tests.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_bar_format_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_bar_format_tests) && (g++ -std=c++23 -Wall -Wextra -Wpedantic -I/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/host-stubs -Iarm9/source -Iarm9/source/gui/views tests/nft2_coverage_renderer_tests.cpp /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/Nft2CoverageRenderer-host.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/nft2_coverage_renderer_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/nft2_coverage_renderer_tests) && (g++ -std=c++23 -Wall -Wextra -Wpedantic -Iarm9/source tests/status_strip_compositor_tests.cpp -o /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_strip_compositor_tests && /tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/test-bin/status_strip_compositor_tests) && (PYTHONDONTWRITEBYTECODE=1 python3 -m unittest tests/test_medium9_font.py tests/test_status_bar_assets.py) && (git diff --check)
test_exit_code=0
test_output_hash=sha256:ec20e26c577e82f1094098c10070fc14808da267800355819a86b29c92ed0f5a
status_geometry_output_hash=sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
nft2_coverage_output_hash=sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
status_strip_compositor_output_hash=sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
python_targeted_output_hash=sha256:ec20e26c577e82f1094098c10070fc14808da267800355819a86b29c92ed0f5a
git_diff_check_output_hash=sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
host_stub_sha256=sha256:479248988c8954c0b39c2a6802d6aba4cfdcdb0d546c765bf9ee4573d6a5c97a
host_renderer_translation_sha256=sha256:930bc294090e6fbb8536db1f9dd2a11087a011df4e8138d4e89b13006deb8627
build_command=podman run --rm --network=none --security-opt label=disable --entrypoint /bin/sh --mount type=bind,src=/home/guill3/Documents/Hobbies/dspico/pico-launcher,dst=/source,ro=true --mount type=bind,src=/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/build,dst=/workspace -w /workspace docker.io/skylyrac/blocksds@sha256:7bea1d0b6026b27036e16ab089770e9864f5844848ccf51e1882d278be116650 -c 'set -eu; cp -a /source/. /workspace/; make clean; make -j1; sha256sum LAUNCHER.nds; stat -c "%s %n" LAUNCHER.nds'
build_exit_code=0
build_output_hash=sha256:4a4cd8198ba08a6fcb5f941d7355dadccd193c134e4a271c4ebebfa56d7f4882
blocksds_image=docker.io/skylyrac/blocksds@sha256:7bea1d0b6026b27036e16ab089770e9864f5844848ccf51e1882d278be116650
blocksds_local_image_id=sha256:bbc583158bf188b895974462ad7a950226406e96004b385ce4ead156327abcc5
blocksds_ndstool_version=1.16.0
build_rom_path=/tmp/opencode/pico-launcher-post-vf001-final-verify-20260814/build/LAUNCHER.nds
build_rom_bytes=542208
build_rom_sha256=54dd7628b76a03df312a54125ebc627b5fdc448eb292d32016adc1b8180da585
accepted_rom_path=/tmp/pico-launcher-nickname-right-two/LAUNCHER.nds
accepted_rom_bytes=542208
accepted_rom_sha256=23699aefafdd6d0b8691e8b9017898e2d431b1e517ac5334768c027ba9b5c73e
accepted_physical_scope=final_observed_configuration_only
physical_matrix=not_executed_not_passed
hardware=not_run
sd=not_accessed
blockers=0
critical_findings=0
verdict=pass_with_warnings
```

### Verification Result Contract

```yaml
status: success
executive_summary: Fresh independent Strict-TDD verification proves VF-001 remediated with 5/5 requirements and 8/8 scenarios compliant; only documented warnings remain.
artifacts:
  - openspec/changes/compose-status-text-on-theme-background/verify-report.md
  - sdd/compose-status-text-on-theme-background/verify-report
next_recommended: none
risks: Native settlement is still orchestrator-owned; limited physical acceptance and the unexecuted 20-cell matrix remain explicit warnings.
skill_resolution: paths-injected
native_attempt_token: sha256:4f4a676d76f717a450c4585fd23153203c6cb4b30acb22d4bf03a27178450964
native_attempt_acquired: true
native_attempt_acquired_by_executor: false
native_attempt_settled: false
```
