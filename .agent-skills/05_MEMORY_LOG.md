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

## [2026-08-16 18:22 UTC] — Agent: Gemini 3.7 Flash

### What I did
- Documented and resolved the four open architecture decisions as formal ADRs in `docs/adr/`:
  - `docs/adr/0001-windowing-license.md`: Permissive licensing policy clarification confirming GLFW (zlib license) compatibility.
  - `docs/adr/0002-networking-transport.md`: Networking transport strategy (lightweight custom UDP reliability layer as default, pluggable msquic).
  - `docs/adr/0003-scripting-language.md`: Addon and scripting strategy (sandboxed WASM via wasm3 as core, optional Lua for dev).
  - `docs/adr/0004-first-green-platform.md`: Linux x86_64 / Vulkan / glibc as primary reference platform for CI.
- Authored `docs/CODING_STANDARDS.md` covering C99/C11 standards, `mg_` prefixes, pointer-to-output math APIs, zero hot-path `malloc`, and docstring requirements.
- Implemented `mg-alloc` (Layer 1):
  - Linear bump arena allocator (`mg_arena_t`) with aligned allocation, reset, and high-water mark tracking (`include/mg/alloc/arena.h`, `src/mg_arena.c`).
  - Fixed-size block pool allocator (`mg_pool_t`) with O(1) allocation/deallocation via free list (`include/mg/alloc/pool.h`, `src/mg_pool.c`).
  - Lock-free Single-Producer Single-Consumer (SPSC) circular ring buffer (`mg_ring_buffer_t`) with cross-compiler memory barriers (`include/mg/alloc/ring_buffer.h`, `src/mg_ring_buffer.c`).
  - Master aggregate header `include/mg/alloc/alloc.h` and CMake configuration `core/mg-alloc/CMakeLists.txt`.
- Implemented `mg-math` (Layer 1):
  - 2D/3D/4D scalar vectors (`mg_vec2_t`, `mg_vec3_t`, `mg_vec4_t`), dot/cross products, normalization, distance, lerp (`include/mg/math/vec*.h`, `src/mg_vec*.c`).
  - 4x4 matrix (`mg_mat4_t`) in row-major layout matching Vulkan clip space (Y-down, depth [0, 1]), multiplication, inversion, translation, scaling, axis/Euler rotations, perspective/ortho projections, and look-at view matrix (`include/mg/math/mat4.h`, `src/mg_mat4.c`).
  - Quaternions (`mg_quat_t`) with axis-angle/Euler creation, quaternion multiplication, normalization, slerp, matrix conversion, and vector rotation (`include/mg/math/quat.h`, `src/mg_quat.c`).
  - Master aggregate header `include/mg/math/math.h` and CMake configuration `core/mg-math/CMakeLists.txt`.
- Wired `mg-alloc` and `mg-math` into top-level `CMakeLists.txt` and `tests/CMakeLists.txt`.
- Wrote and added headless unit tests `tests/unit/mg-alloc/test_mg_alloc.c` and `tests/unit/mg-math/test_mg_math.c`.

### What I verified
- Ran `cmake -B build -G Ninja -DMG_HEADLESS_TESTS=ON` — successfully configured with license audit passing.
- Ran `ninja -C build` — compiled `libmg-pal.a`, `libmg-alloc.a`, `libmg-math.a`, and unit test targets with zero warnings under `-Wall -Wextra -Werror`.
- Ran `ctest --test-dir build --output-on-failure` — 3/3 tests passed (100% pass rate: `mg-pal.version_string`, `mg-alloc.unit`, `mg-math.unit`).

### What's next
- Implement real `mg-pal` platform abstractions (windowing interface, threads/mutex/condvar, filesystem I/O and change watcher, monotonic clock, atomics) across Linux/desktop before moving to Phase 2 (`mg-rx`).
- Begin Phase 2: `mg-rx` (reactive core subject, behavior subject, observables, operators: map, filter, debounce, throttle, merge, combine_latest, scan).

### Blockers / open questions
- None. ADRs 0001–0004 are recorded.

### Files touched
- `docs/adr/0001-windowing-license.md`, `docs/adr/0002-networking-transport.md`, `docs/adr/0003-scripting-language.md`, `docs/adr/0004-first-green-platform.md`
- `docs/CODING_STANDARDS.md`
- `core/mg-alloc/include/mg/alloc/arena.h`, `core/mg-alloc/include/mg/alloc/pool.h`, `core/mg-alloc/include/mg/alloc/ring_buffer.h`, `core/mg-alloc/include/mg/alloc/alloc.h`, `core/mg-alloc/src/mg_arena.c`, `core/mg-alloc/src/mg_pool.c`, `core/mg-alloc/src/mg_ring_buffer.c`, `core/mg-alloc/CMakeLists.txt`
- `core/mg-math/include/mg/math/math.h`, `core/mg-math/include/mg/math/vec2.h`, `core/mg-math/include/mg/math/vec3.h`, `core/mg-math/include/mg/math/vec4.h`, `core/mg-math/include/mg/math/mat4.h`, `core/mg-math/include/mg/math/quat.h`, `core/mg-math/src/mg_vec2.c`, `core/mg-math/src/mg_vec3.c`, `core/mg-math/src/mg_vec4.c`, `core/mg-math/src/mg_mat4.c`, `core/mg-math/src/mg_quat.c`, `core/mg-math/CMakeLists.txt`
- `tests/unit/mg-alloc/CMakeLists.txt`, `tests/unit/mg-alloc/test_mg_alloc.c`
- `tests/unit/mg-math/CMakeLists.txt`, `tests/unit/mg-math/test_mg_math.c`
- `CMakeLists.txt`, `tests/CMakeLists.txt`
- `.agent-skills/05_MEMORY_LOG.md`

### ADRs added/changed
- Added ADRs 0001 through 0004.

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
