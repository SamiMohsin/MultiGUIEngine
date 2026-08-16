# MultiGUIEngine — Complete Ground-Up Project Specification

> **Purpose of this document:** This is a self-contained implementation spec for an AI coding model (or human developer) to build MultiGUIEngine from an empty repository to a working engine, with no external context required beyond what's written here. Every section is written to be actionable: read it top to bottom, execute in order.

---

## 0. Project Identity

- **Name:** MultiGUIEngine
- **Owner/Repo:** github.com/SamiMohsin/MultiGUIEngine
- **One-line description:** A from-scratch, cross-platform 2D/2.5D/3D game and application engine written in C with performance-critical paths in Assembly, using only MIT/permissive-licensed third-party code, with a built-in reactive core, plugin/addon system, and local AI model integration.
- **Primary language:** C (C99/C11)
- **Secondary language:** Assembly (x86-64 AVX2 for desktop, ARM NEON for mobile/embedded) — only for profiler-confirmed hotspots, never written speculatively.
- **Build system:** CMake ≥ 3.20 + Ninja generator.
- **License of the engine itself:** MIT.
- **License policy for dependencies:** Only MIT, BSD, zlib, Apache-2.0, or Public Domain. No LGPL/GPL anywhere in the link graph. Every dependency must be logged in `THIRD_PARTY_LICENSES.md` with SPDX identifier before it's added.

---

## 1. Target Platforms

| Platform | Minimum version | Graphics backend | Notes |
|---|---|---|---|
| Linux | any modern distro, glibc | Vulkan 1.2 | primary dev platform |
| Windows | 10+ | Vulkan 1.2 | MinGW-w64 or MSVC toolchain |
| macOS | 12+ | Metal | Objective-C shim only for windowing/lifecycle |
| Android | API 26+ | Vulkan / GLES 3.2 fallback | NDK r26+ |
| iOS | 15+ | Metal | Swift/ObjC shim for lifecycle only |
| Embedded | RP2040, ESP32-class (≥520KB RAM) | custom software rasterizer | separate build profile, no ECS archetype overhead |

---

## 2. Non-Negotiable Architectural Rules

1. **Layered dependency direction only.** A module may depend only on modules below it in the layer stack (Section 3). No lateral calls except through the reactive event bus.
2. **No malloc/free in the per-frame hot path.** All per-frame allocations come from arena or pool allocators reset each frame.
3. **Every engine module must build and pass tests headless** (no window, no GPU) via a `--headless` CMake target, except the renderer itself which gets a swiftshader/CPU-fallback test path.
4. **Correctness before optimization.** Every module ships a scalar/reference C implementation first. SIMD/Assembly variants are added only after a profiler run (documented in `/docs/profiling/`) proves a bottleneck, and the scalar path stays as a runtime fallback.
5. **Hot-reloadable config.** Scene, material, and UI definitions live in RON-like or JSON text files, loaded and re-loaded without recompilation.
6. **No cloud dependency required to build or run.** AI features must work fully offline against a local model server.
7. **Every third-party library addition requires a one-paragraph justification comment in `THIRD_PARTY_LICENSES.md`**: why this library, why this license is safe, what it's used for.

---

## 3. Layer Stack (top depends on bottom only)

```
L6  App/Game Layer         — user projects built on the engine
L5  Plugin & Addon Host    — native ABI plugins + sandboxed WASM addons
L4  AI Bridge              — local LLM integration
L3  Engine Modules         — render, scene/ECS, physics, audio, ui, input, net, asset
L2  Reactive Core (mgrx)   — Observable/Subject/Stream event bus
L1  Platform Abstraction   — windowing, threads, filesystem, time, atomics
```

---

## 4. Repository Layout (create exactly this structure first)

```
MultiGUIEngine/
├── CMakeLists.txt                  # top-level, options, subdirectory includes
├── cmake/
│   ├── toolchains/                 # per-platform toolchain files
│   ├── FindModules.cmake
│   └── LicenseAudit.cmake          # CI check: fails build on non-permissive license
├── THIRD_PARTY_LICENSES.md
├── LICENSE                         # MIT
├── docs/
│   ├── ARCHITECTURE.md
│   ├── CODING_STANDARDS.md
│   ├── profiling/
│   └── adr/                        # architecture decision records, one file per decision
├── core/
│   ├── mg-pal/                     # platform abstraction layer
│   │   ├── include/mg/pal/
│   │   └── src/{linux,windows,macos,android,ios,embedded}/
│   ├── mg-rx/                      # reactive core
│   ├── mg-math/                    # vec/mat/quat, scalar first
│   └── mg-alloc/                   # arena/pool/frame allocators
├── modules/
│   ├── mg-render/
│   │   ├── include/mg/render/
│   │   └── src/{backend_vulkan,backend_metal,backend_gles,backend_software}/
│   ├── mg-scene/                   # ECS
│   ├── mg-physics/
│   ├── mg-audio/
│   ├── mg-ui/
│   ├── mg-input/
│   ├── mg-net/
│   └── mg-asset/
├── ai-bridge/
│   ├── mg-ai-core/                 # model router
│   └── mg-ai-tools/                # NPC/procgen/vision helpers
├── plugin-host/
│   ├── mg-plugin-abi/              # stable C ABI + versioned vtable
│   └── mg-wasm-host/               # wasm3-based sandboxed addon runtime
├── platform/
│   └── {linux,windows,macos,android,ios,embedded}/  # per-OS entry points, packaging
├── third_party/                    # vendored MIT-only deps as git submodules
├── tools/
│   ├── mg-editor/                  # visual editor shell
│   ├── mg-cook/                    # asset pipeline CLI
│   └── mg-profiler/
├── tests/
│   ├── unit/                       # one dir per module, mirrors modules/
│   └── headless_harness/
├── examples/
│   ├── 01_triangle/
│   ├── 02_sprite_batch/
│   ├── 03_ecs_scene/
│   ├── 04_3d_forward_plus/
│   ├── 05_ui_app/
│   └── 06_ai_npc_demo/
└── .github/workflows/ci.yml
```

---

## 5. Third-Party Dependencies (pin exact versions when vendoring)

| Purpose | Library | License | Vendored as |
|---|---|---|---|
| Windowing + input | GLFW | zlib/libpng (permissive) | `third_party/glfw` |
| Vulkan function loader | volk | MIT | `third_party/volk` |
| Shader cross-compilation | SPIRV-Cross | Apache-2.0 | `third_party/spirv-cross` |
| Shader compilation (GLSL→SPIR-V) | glslang | BSD-3-Clause/Apache-2.0 | `third_party/glslang` |
| Font rasterization | stb_truetype.h | Public Domain / MIT | `third_party/stb` |
| Image loading | stb_image.h | Public Domain / MIT | `third_party/stb` |
| Audio decoding | dr_wav.h, dr_mp3.h, dr_flac.h | Public Domain / MIT | `third_party/dr_libs` |
| WASM runtime (addons) | wasm3 | MIT | `third_party/wasm3` |
| Local LLM inference | llama.cpp | MIT | external process, not vendored — invoked via HTTP or subprocess |
| JSON parsing | hand-written (no dependency) | — | `core/mg-pal/src/json/` |

Do not add any dependency not on this list without first adding a row here and confirming license compatibility.

---

## 6. Module Specifications

### 6.1 `mg-pal` (Platform Abstraction Layer)

**Public headers:** `mg/pal/window.h`, `mg/pal/thread.h`, `mg/pal/fs.h`, `mg/pal/time.h`, `mg/pal/atomic.h`

**Responsibilities:**
- Window creation/destruction, event polling (wraps GLFW on desktop; native APIs on mobile/embedded)
- Thread creation, mutex, condvar, thread-safe queue primitives
- Filesystem: read/write files, watch-for-changes (for hot reload), path normalization across OSes
- High-resolution monotonic clock
- Atomic ops wrapper (falls back to compiler intrinsics per-platform)

**API sketch:**
```c
typedef struct mg_window mg_window_t;
mg_window_t* mg_window_create(const mg_window_desc_t* desc);
void mg_window_destroy(mg_window_t* w);
bool mg_window_poll_events(mg_window_t* w); // returns false when close requested

typedef struct mg_thread mg_thread_t;
mg_thread_t* mg_thread_spawn(void (*fn)(void*), void* arg);
void mg_thread_join(mg_thread_t* t);
```

**Test requirement:** headless test creates a window in offscreen mode (or skips window creation on CI) and validates event queue plumbing with synthetic events.

---

### 6.2 `mg-rx` (Reactive Core)

**Public headers:** `mg/rx/subject.h`, `mg/rx/observable.h`, `mg/rx/operators.h`

**Core types:**
```c
typedef struct mg_subject mg_subject_t;
typedef struct mg_observable mg_observable_t;
typedef struct mg_subscription mg_subscription_t;

mg_subject_t* mg_subject_create(void);
mg_subscription_t* mg_subject_subscribe(mg_subject_t* s, void (*on_next)(void* value, void* ctx), void* ctx);
void mg_subject_emit(mg_subject_t* s, void* value);
void mg_subscription_dispose(mg_subscription_t* sub);

// behavior subject: holds last value, replays to new subscribers
typedef struct mg_behavior_subject mg_behavior_subject_t;
mg_behavior_subject_t* mg_behavior_subject_create(void* initial_value);
```

**Operators to implement (each returns a new `mg_observable_t*` wrapping the source):**
`mg_rx_map`, `mg_rx_filter`, `mg_rx_debounce_ms`, `mg_rx_throttle_ms`, `mg_rx_merge`, `mg_rx_combine_latest`, `mg_rx_scan`

**Threading model:** lock-free SPSC ring buffer (`mg_ring_buffer_t` in `mg-alloc`) used to move events from producer threads (input, net, asset watcher) into the main-thread event bus, drained once per frame at a fixed point in the game loop.

**Test requirement:** every operator gets a unit test with synthetic emit sequences and expected output sequences, run headless.

---

### 6.3 `mg-math`

Scalar-first implementations of: `vec2`, `vec3`, `vec4`, `mat4`, `quat`. Row-major, right-handed coordinate system, matches Vulkan clip space (Y-down, depth 0..1).

**API convention:** all functions take pointers to output, never return structs by value (avoids ABI ambiguity across compilers), e.g.:
```c
void mg_mat4_mul(mg_mat4_t* out, const mg_mat4_t* a, const mg_mat4_t* b);
void mg_vec3_normalize(mg_vec3_t* out, const mg_vec3_t* v);
```

SIMD (AVX2/NEON) variants live in `mg-math/src/simd/` behind a runtime CPU-feature detection dispatch table, added only after Phase 12 profiling.

---

### 6.4 `mg-alloc`

- `mg_arena_t`: linear bump allocator, reset per frame (`mg_arena_reset`)
- `mg_pool_t`: fixed-size block pool for ECS components
- `mg_ring_buffer_t`: lock-free SPSC ring buffer for cross-thread event passing

No allocator here calls `malloc` after initialization; all memory is reserved up front from OS virtual memory and carved out.

---

### 6.5 `mg-render`

**RHI (Render Hardware Interface):** a backend-agnostic C API (`mg/render/rhi.h`) with function-pointer-table backends selected at init time:

```c
typedef struct mg_rhi_backend {
    bool (*init)(const mg_rhi_desc_t* desc);
    void (*shutdown)(void);
    mg_buffer_handle_t (*create_buffer)(const mg_buffer_desc_t*);
    mg_texture_handle_t (*create_texture)(const mg_texture_desc_t*);
    mg_pipeline_handle_t (*create_pipeline)(const mg_pipeline_desc_t*);
    void (*begin_frame)(void);
    void (*draw)(const mg_draw_cmd_t*);
    void (*end_frame)(void);
} mg_rhi_backend_t;
```

**Backends to implement, in this order:**
1. `backend_software` — CPU rasterizer, used for embedded + headless CI tests. Build this first; it's the reference implementation every other backend's output is diffed against.
2. `backend_vulkan` — Linux/Windows/Android, via volk.
3. `backend_metal` — macOS/iOS, Objective-C++ translation unit bridging to the C API.
4. `backend_gles` — Android fallback for devices without Vulkan.

**Rendering tiers built on top of RHI:**
- **2D:** batched quad/sprite renderer, texture atlas packer, SDF text via stb_truetype
- **2.5D:** depth-sorted sprite layers + billboarded/tilted 3D primitives sharing the 2D batcher where possible
- **3D:** forward+ clustered renderer, PBR material model (metallic/roughness), shadow maps (single cascade to start)

**Shader pipeline:** author shaders in GLSL, compile to SPIR-V via glslang at build time (`mg-cook` step), cross-compile to MSL for Metal via SPIRV-Cross. Ship compiled bytecode, not source, in release builds.

---

### 6.6 `mg-scene` (ECS)

Archetype-based ECS (components stored in contiguous arrays grouped by archetype, not sparse per-entity maps) for cache efficiency.

**Core API:**
```c
typedef struct mg_world mg_world_t;
typedef uint64_t mg_entity_t;

mg_world_t* mg_world_create(void);
mg_entity_t mg_entity_create(mg_world_t* w);
void mg_entity_destroy(mg_world_t* w, mg_entity_t e);

void mg_component_register(mg_world_t* w, mg_component_id_t id, size_t size);
void* mg_component_add(mg_world_t* w, mg_entity_t e, mg_component_id_t id);
void* mg_component_get(mg_world_t* w, mg_entity_t e, mg_component_id_t id);

typedef void (*mg_system_fn)(mg_world_t* w, float dt, void* ctx);
void mg_system_register(mg_world_t* w, mg_system_fn fn, void* ctx, mg_stage_t stage);
```

**Stages, run in this fixed order per frame:** `INPUT → FIXED_UPDATE (physics, may run 0..N times to catch up) → UPDATE → LATE_UPDATE → RENDER_EXTRACT → RENDER`

Scene files (`.mgscene`, JSON-based) hot-reload by diffing against the live world and only touching changed entities.

---

### 6.7 `mg-physics`

- Broadphase: BVH (bounding volume hierarchy), incrementally updated.
- Narrowphase: SAT for convex polygons/boxes (2D and 3D), GJK+EPA for general convex 3D shapes.
- Solver: sequential impulse solver, fixed timestep (default 60Hz), decoupled from render framerate via the FIXED_UPDATE ECS stage.
- 2D and 3D share the broadphase/solver code where possible; only shape/collision-manifold code diverges.

---

### 6.8 `mg-audio`

- Mixer graph: source → optional DSP nodes (gain, pan, low-pass) → master bus
- Decoders via dr_wav/dr_mp3/dr_flac
- Streaming playback for long files (music), fully-loaded playback for short SFX
- Runs on a dedicated audio thread, fed via `mg-rx` ring buffer from the main thread for control messages (play/stop/volume)

---

### 6.9 `mg-ui`

Hybrid immediate-mode (for editor/tool panels) + retained-mode (for in-game/app UI bound to ECS state via `mg-rx`) toolkit.

- Widget primitives: panel, label, button, text input, slider, image, list, scroll container
- Layout: flexbox-inspired constraint solver (row/column, grow/shrink, gap, padding)
- Styling: theme struct (colors, fonts, spacing) loaded from JSON, hot-reloadable
- App-mode packaging: `mg-cook` can strip render-3D/physics/audio at link time via CMake feature flags (`MG_ENABLE_3D=OFF` etc.) so a UI-only app binary stays small

---

### 6.10 `mg-input`

Unified input event model across keyboard/mouse/touch/gamepad, emitted as `mg-rx` observables (`mg_input_bus_key_down`, `mg_input_bus_pointer_move`, etc.) rather than polled, so both game code and UI can subscribe without polling collisions.

---

### 6.11 `mg-net`

- Transport: custom UDP + reliability layer, or msquic (MIT) if QUIC is desired for NAT traversal and stream multiplexing — decide in ADR before Phase 7.
- Model: authoritative server, client-side prediction + server reconciliation for player-controlled entities, snapshot interpolation for remote entities.
- Serialization: hand-written binary packer (component data → wire format), versioned per-message for forward compatibility.

---

### 6.12 `mg-asset`

- Import pipeline (`mg-cook` CLI): source assets (png/wav/gltf/etc.) → cooked engine-native formats (compressed textures, packed audio, binary mesh format)
- Runtime loader: async I/O via `mg-pal` thread pool, reference-counted handles, hot-reload via filesystem watcher feeding an `mg-rx` subject

---

## 7. Plugin & Addon System

### 7.1 Native Plugins (`mg-plugin-abi`)

Stable, versioned C ABI. Every plugin exports one entry point:
```c
typedef struct mg_plugin_vtable {
    uint32_t abi_version;   // must match MG_PLUGIN_ABI_VERSION
    bool (*on_load)(mg_engine_ctx_t* ctx);
    void (*on_register_systems)(mg_world_t* world);
    void (*on_tick)(float dt);
    void (*on_unload)(void);
} mg_plugin_vtable_t;

MG_PLUGIN_EXPORT mg_plugin_vtable_t* mg_plugin_entry(void);
```
Discovered via `plugins/<name>/plugin.json` manifest (name, version, abi_version, entry library path, requested capabilities). Loaded with `dlopen`/`LoadLibrary`/`dlopen` (macOS) at engine startup.

### 7.2 Sandboxed Addons (`mg-wasm-host`)

- Runtime: wasm3
- Capability model: addon manifest declares required host functions (e.g. `scene.spawn_entity`, `ui.create_panel`) explicitly; host only exposes the requested subset
- No raw filesystem or network access unless explicitly granted per-manifest and confirmed by the user at install time

---

## 8. AI Bridge (`mg-ai-core`, `mg-ai-tools`)

- **Model router:** connects to a local model server — Ollama (`http://localhost:11434/v1`) or a `llama-server` subprocess (`http://localhost:8080/v1`) — via HTTP, OpenAI-compatible chat completion schema. No cloud API keys anywhere in this module.
- **Streaming:** responses arrive as SSE tokens, pushed into an `mg-rx` subject so any subscriber (editor console, in-game dialogue box) can consume them live.
- **Critical-path isolation rule:** physics/render/input never block on an AI call. AI output only ever *proposes* data (behavior tree, dialogue text, generated asset) that is consumed on a later frame once ready — never awaited synchronously inside a game-loop stage.
- **Use cases to implement, in order:**
  1. Dialogue/text generation demo (`examples/06_ai_npc_demo`)
  2. NPC behavior-tree proposal: model outputs a small JSON behavior-tree spec, engine's native BT interpreter executes it (interpreter is engine code, not the model)
  3. Editor codegen assist: scene graph → exportable Lua/WASM addon stub

---

## 9. Coding Standards (`docs/CODING_STANDARDS.md` — write this file verbatim)

- C99, compiled with `-Wall -Wextra -Werror` (or `/W4 /WX` on MSVC)
- No exceptions to the "no hot-path malloc" rule; violations block PR merge
- Naming: `mg_` prefix for all public symbols, `snake_case` functions, `mg_type_t` for typedefs
- Every public header gets a doc-comment block per function (params, return, ownership of pointers)
- Every module ships with a `tests/unit/<module>/` directory; CI fails if coverage drops below the previous commit's baseline
- No global mutable state except explicitly named singletons documented in `docs/adr/`

---

## 10. Build & CI

`CMakeLists.txt` top-level options:
```cmake
option(MG_ENABLE_3D "Build 3D renderer/physics" ON)
option(MG_ENABLE_NET "Build networking module" ON)
option(MG_ENABLE_AI "Build AI bridge" ON)
option(MG_ENABLE_WASM_ADDONS "Build WASM addon host" ON)
option(MG_HEADLESS_TESTS "Build headless test harness" ON)
option(MG_TARGET_EMBEDDED "Build embedded profile (disables ECS archetypes, uses flat SoA)" OFF)
```

`.github/workflows/ci.yml` matrix: `{linux, windows, macos} x {debug, release}`, plus a separate `license-audit` job running `cmake/LicenseAudit.cmake` that scans `third_party/` and fails on any non-permissive SPDX tag.

---

## 11. Build Order — Execute Phases Strictly in Sequence

Each phase must end with: all unit tests passing headless, a tagged git commit, and an entry appended to `docs/adr/` if any architectural decision was made.

**Phase 0 — Bootstrap**
Create the repo layout from Section 4. Top-level `CMakeLists.txt` that configures but builds nothing yet except an empty `mg-pal` stub. Set up `THIRD_PARTY_LICENSES.md` and CI skeleton.

**Phase 1 — `mg-pal` + `mg-alloc` + `mg-math`**
Implement windowing (GLFW-backed on desktop), thread primitives, arena/pool allocators, scalar math library. Headless unit tests for all three.

**Phase 2 — `mg-rx`**
Full reactive core with all operators listed in 6.2. Unit tests with synthetic emit/expect sequences.

**Phase 3 — `mg-render` software backend + 2D batcher**
CPU rasterizer first (this is the cross-checked reference). Then batched sprite/quad renderer on top. Validate with `examples/01_triangle` and `examples/02_sprite_batch`.

**Phase 4 — `mg-scene` (ECS)**
Archetype storage, entity/component API, system scheduler with the fixed stage order. `examples/03_ecs_scene`.

**Phase 5 — `mg-render` Vulkan backend**
Bring up Vulkan on Linux first, then Windows, then Android. Diff output against the software backend for correctness.

**Phase 6 — 2.5D + 3D rendering**
Depth-sorted 2.5D layer, forward+ 3D pipeline, PBR materials, single-cascade shadow map. `examples/04_3d_forward_plus`.

**Phase 7 — `mg-physics`**
BVH broadphase, SAT/GJK narrowphase, sequential impulse solver, FIXED_UPDATE integration into `mg-scene`.

**Phase 8 — `mg-audio`**
Mixer graph, dr_libs decoders, dedicated audio thread wired through `mg-rx`.

**Phase 9 — `mg-input` + `mg-ui`**
Unified input event bus, then the UI toolkit (immediate + retained), theme JSON loading, `examples/05_ui_app`.

**Phase 10 — `mg-net`**
Transport decision recorded in an ADR, then client/server loop, prediction/reconciliation, snapshot interpolation.

**Phase 11 — `mg-asset` + `mg-cook`**
Import pipeline, cooked binary formats, async runtime loader, hot-reload via filesystem watcher.

**Phase 12 — `plugin-host` (native ABI + WASM)**
Native plugin loader with manifest discovery. wasm3 sandboxed addon runtime with capability-scoped host function exposure.

**Phase 13 — `mg-ai-core` + `mg-ai-tools`**
Model router against local Ollama/llama-server, streaming token bus, `examples/06_ai_npc_demo`.

**Phase 14 — Platform ports**
macOS/iOS Metal backend, Android NDK packaging, embedded profile (`MG_TARGET_EMBEDDED=ON`) on RP2040/ESP32-class hardware.

**Phase 15 — `mg-editor` tool**
Visual editor shell consuming the engine as a library: scene tree, node-graph shader/behavior authoring, asset browser, AI console panel.

**Phase 16 — Profiling & SIMD pass**
Run `mg-profiler` against representative scenes, document findings in `docs/profiling/`, add AVX2/NEON kernels only where justified, keeping scalar fallback paths intact.

---

## 12. Definition of Done (per module)

A module is complete when all of the following are true:
- [ ] Builds headless on Linux, Windows, macOS in CI
- [ ] Public header has doc comments on every exported function
- [ ] Unit tests exist and pass, covering the operator/API surface described in Section 6
- [ ] No dynamic allocation in any function called from a per-frame stage
- [ ] Added to `THIRD_PARTY_LICENSES.md` if it pulled in any new dependency
- [ ] At least one `examples/` project exercises it end-to-end
- [ ] An ADR exists for any decision that could plausibly have gone another way

---

## 13. Open Decisions to Resolve Before Phase 0 (write these as the first ADRs)

1. Windowing library: GLFW (chosen above, zlib license) — confirm acceptable under "MIT-only" policy or adjust policy wording to "MIT/permissive."
2. Networking transport: custom UDP+reliability vs msquic (MIT).
3. Gameplay scripting language for hot-reloadable logic: Lua (MIT) vs WASM-only.
4. First platform to reach "fully green CI": Linux, recommended.

Resolve these four as `docs/adr/0001` through `0004` before writing any Phase 0 code.
