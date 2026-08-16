# Memory Log

**Purpose:** This is the persistent, model-agnostic memory of the project. Every agent — Claude, Gemini, local Qwen/Mistral/Devstral, or anything else — reads the latest entry here before starting work, and appends a new entry before ending its session. This file is the source of truth for "what happened and what's next," not any individual model's internal memory.

**Format:** newest entry at the top. Never delete old entries — this is an append-only log. If something described in an old entry later turns out to be wrong, add a correction note in a new entry; don't edit history.

---

## Entry template (copy this for every new entry)

```
## [YYYY-MM-DD HH:MM UTC] — Agent: <model name/version>

### What I did
- 

### What I verified (tests run, builds confirmed — be specific, don't claim what you didn't check)
- 

### What's next (concrete, actionable — the next agent should be able to start immediately)
- 

### Blockers / open questions
- 

### Files touched
- 

### ADRs added/changed
- 
```

---

## [2026-08-16 16:52 UTC] — Agent: Claude Sonnet 5 (Phase 0 build session)

### What I did
- Created the full Phase 0 repository folder structure per `docs/ARCHITECTURE.md` Section 4 (core/, modules/, ai-bridge/, plugin-host/, platform/, third_party/, tools/, tests/, examples/, .github/workflows/).
- Wrote top-level `CMakeLists.txt` with all build options from Section 10, `-Wall -Wextra -Werror` / `/W4 /WX`, and commented-out `add_subdirectory()` calls for every future-phase module so the build order stays visible in one file.
- Wrote `cmake/LicenseAudit.cmake` — a real, working CMake function that scans `third_party/` against `THIRD_PARTY_LICENSES.md` and fails configure on an undocumented vendored library.
- Wrote `THIRD_PARTY_LICENSES.md` with the pre-approved dependency table from spec Section 5 (none vendored yet, so the audit currently passes trivially).
- Wrote `LICENSE` (MIT).
- Implemented the `mg-pal` Phase 0 stub: `include/mg/pal/version.h` + `src/mg_pal_stub.c`, exposing `mg_pal_version_string()`. This is intentionally minimal — real windowing/thread/fs/time APIs are Phase 1 work, not done here.
- Wrote a headless unit test (`tests/unit/mg-pal/test_mg_pal.c`) and wired it into CTest.
- Wrote `.github/workflows/ci.yml`: a `license-audit` job followed by a `{ubuntu, windows, macos} x {Debug, Release}` build+test matrix job.
- Wrote `AGENTS.md` (root-level pointer into `.agent-skills/`) and `README.md`.

### What I verified
- Installed `cmake` 3.28.3 and `ninja` in this sandbox.
- Ran `cmake -G Ninja ..` from a clean `build/` dir — configured successfully, license audit step printed "all vendored third_party/ entries accounted for."
- Ran `ninja` — built `libmg-pal.a` and `test_mg_pal` with zero warnings under `-Wall -Wextra -Werror`.
- Ran `ctest --output-on-failure` — 1/1 tests passed (`mg-pal.version_string`).
- Did **not** run the GitHub Actions workflow itself (no CI runner available in this sandbox) — the YAML is written but unverified on actual GitHub infrastructure. Flagging this explicitly per the no-hallucinated-completion rule.
- Did **not** test on Windows/macOS/Android/iOS/embedded — only Linux/gcc verified.

### What's next
- Push this scaffold to `github.com/SamiMohsin/MultiGUIEngine`, confirm the CI workflow actually runs green on real GitHub Actions infrastructure (this was not verified here).
- Resolve the four open decisions in `docs/ARCHITECTURE.md` Section 13 as ADRs `0001`–`0004` (windowing library license wording, networking transport, gameplay scripting language, first-platform-green priority) — still outstanding from the prior session, still blocking real Phase 1 work.
- Begin Phase 1 per spec Section 11: implement real `mg-pal` windowing/thread/fs/time/atomic APIs (currently only a version stub), plus `mg-alloc` (arena/pool/ring-buffer) and `mg-math` (scalar vec/mat/quat). Uncomment their `add_subdirectory()` lines in the top-level `CMakeLists.txt` as each lands.

### Blockers / open questions
- Same four open ADR decisions as last entry — still need Ayoob's sign-off before Phase 1 windowing/networking/scripting code is written.
- GitHub Actions matrix (Windows/macOS runners) is unverified — first real push should be watched closely for platform-specific build breaks.

### Files touched
- `CMakeLists.txt`, `cmake/LicenseAudit.cmake`, `THIRD_PARTY_LICENSES.md`, `LICENSE`, `README.md`, `AGENTS.md`
- `core/mg-pal/CMakeLists.txt`, `core/mg-pal/include/mg/pal/version.h`, `core/mg-pal/src/mg_pal_stub.c`
- `tests/CMakeLists.txt`, `tests/unit/mg-pal/CMakeLists.txt`, `tests/unit/mg-pal/test_mg_pal.c`
- `.github/workflows/ci.yml`
- Full empty folder scaffold for every future module/phase (no code yet in most — see Section 4 of the spec for the full tree)

### ADRs added/changed
- None yet — still queued (0001–0004), same as prior entry.

---

## [2026-08-16 00:00 UTC] — Agent: Claude Sonnet 5 (setup session)

### What I did
- Produced the full ground-up project specification (`docs/ARCHITECTURE.md`): repo layout, module API sketches, dependency table, 16-phase build order, definition-of-done checklist.
- Produced this agent-skills folder (`00_MASTER_RULES.md` through `06_COMMIT_RULES.md`) to govern how any AI model contributes to this repo.

### What I verified
- Nothing built or run yet — this session was pure specification/documentation, no code written. No compiler was invoked.

### What's next
- Phase 0 per `docs/ARCHITECTURE.md` Section 11: create the actual repo folder structure, top-level `CMakeLists.txt` (configures but builds nothing except an empty `mg-pal` stub), `THIRD_PARTY_LICENSES.md`, and CI skeleton (`.github/workflows/ci.yml`).
- Resolve the four open decisions in `docs/ARCHITECTURE.md` Section 13 as ADRs `0001`–`0004` before writing any Phase 0 code: windowing library license wording, networking transport choice, gameplay scripting language, first platform to reach green CI.

### Blockers / open questions
- The four open decisions above need a human (Ayoob) sign-off before Phase 0 code is written, per `01_ARCHITECTURE_FLOW.md` dependency/license process.

### Files touched
- `docs/ARCHITECTURE.md` (new)
- `.agent-skills/*` (new, this folder)

### ADRs added/changed
- None yet — ADRs 0001–0004 are queued as the next action.
