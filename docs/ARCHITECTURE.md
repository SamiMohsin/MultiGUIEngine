# GENESIS ENGINE — Multi-GUI 2D/2.5D/3D Game & App Engine
### Full Architecture Blueprint & Implementation Roadmap
**Language:** C (core) + Assembly (hot paths) | **License policy:** MIT-only dependencies | **AI:** Local LLM integration (Ollama/llama.cpp-compatible)
 
---
 
## 1. Design Philosophy (consistent with your existing stack)
 
- **Blueprint-first, module-by-module delivery** — each module independently headless-testable, no cross-layer dependency leaks (mirrors your `warden-sim` never depending on `warden-render` rule).
- **forge_rx reactive core** — same Observable/Subject/operator pattern you use in FORGE Coach and WARDEN, written in pure C this time (no RxCpp dependency, since RxCpp itself is BSD but adds weight — we hand-roll it to guarantee zero-license-risk and embedded portability).
- **Correctness before SIMD/ASM** — AVX2/NEON/assembly kernels only after profiling confirms bottlenecks, same discipline as FORGE AI.
- **Zero telemetry, fully offline-capable**, AI model integration optional and swappable (Ollama HTTP or embedded llama.cpp subprocess, MIT-licensed llama.cpp).
- **RON/JSON hot-reloadable config** for scenes, materials, and UI, no recompilation needed.
---
 
## 2. Layered Architecture
 
```
┌─────────────────────────────────────────────────────────────┐
│  LAYER 6: App/Game Layer (user projects, scripting, GUI apps)│
├─────────────────────────────────────────────────────────────┤
│  LAYER 5: Plugin & Addon Host  (dynamic .so/.dll/.dylib +    │
│           WASM sandboxed plugins)                            │
├─────────────────────────────────────────────────────────────┤
│  LAYER 4: AI Bridge  (local LLM: codegen, NPC behavior,      │
│           procedural content, vision-assisted design)        │
├─────────────────────────────────────────────────────────────┤
│  LAYER 3: Engine Modules                                     │
│   ├─ genesis-render   (2D/2.5D/3D unified renderer abstraction)│
│   ├─ genesis-scene    (ECS world, scene graph)                │
│   ├─ genesis-physics  (2D + 3D, broadphase/narrowphase)        │
│   ├─ genesis-audio                                             │
│   ├─ genesis-ui       (immediate + retained GUI toolkit)       │
│   ├─ genesis-input                                              │
│   ├─ genesis-net      (client/server, QUIC-based like WARDEN)  │
│   └─ genesis-asset    (import/cook pipeline)                   │
├─────────────────────────────────────────────────────────────┤
│  LAYER 2: forge_rx Reactive Core (Observable/Subject/Stream)  │
├─────────────────────────────────────────────────────────────┤
│  LAYER 1: Platform Abstraction Layer (PAL)                    │
│   Linux | Windows | macOS | Android | iOS | Embedded (RP2040/ESP32)│
└─────────────────────────────────────────────────────────────┘
```
 
**Rule:** Layer N may only call into Layer N-1 and below. No lateral module-to-module calls except via the reactive event bus (Layer 2). This is enforced at CI time via a symbol-dependency linter script.
 
---
 
## 3. Repository / Workspace Layout
 
```
genesis-engine/
├── core/
│   ├── genesis-rx/          # forge_rx reactive core (C99, header+src)
│   ├── genesis-pal/         # platform abstraction (windowing, threads, fs, time)
│   ├── genesis-math/        # SIMD-ready vec/mat/quat (scalar first)
│   └── genesis-alloc/       # arena/pool/frame allocators (no malloc in hot loop)
├── modules/
│   ├── genesis-render/      # RHI: Vulkan / Metal / D3D12 / GLES backends
│   ├── genesis-scene/       # ECS (archetype-based, cache-friendly)
│   ├── genesis-physics/
│   ├── genesis-audio/
│   ├── genesis-ui/
│   ├── genesis-input/
│   ├── genesis-net/
│   └── genesis-asset/
├── ai-bridge/
│   ├── genesis-ai-core/     # model router (Ollama HTTP / llama.cpp subprocess)
│   └── genesis-ai-tools/    # NPC dialogue, proc-gen, vision-assisted editor
├── plugin-host/
│   ├── genesis-plugin-abi/  # stable C ABI, versioned vtable
│   └── genesis-wasm-host/   # wasm3 or wasmtime (MIT/Apache) sandboxed addons
├── platform/
│   ├── linux/  windows/  macos/  android/  ios/  embedded/
├── tools/
│   ├── genesis-editor/      # your existing oscilloscope/lime themed editor shell
│   ├── genesis-cook/        # asset pipeline CLI
│   └── genesis-profiler/
├── examples/
└── build/  (CMake + Ninja, per-platform toolchains)
```
 
---
 
## 4. Core Reactive System — `genesis-rx`
 
Pure C99 reimplementation of your forge_rx pattern:
 
- `rx_subject_t`, `rx_behavior_subject_t`, `rx_observable_t`
- Operators: `map`, `filter`, `debounce`, `throttle`, `merge`, `combine_latest`, `scan`
- Backpressure: ring-buffer based, lock-free SPSC queues between engine threads (render/physics/audio/net) and the main event bus
- Used for: input events, asset hot-reload triggers, UI state binding, AI-response streaming, plugin lifecycle events
This becomes the **spine** connecting every module — same architectural role RxJS plays in QUANTCHART and LocalVsCodeModel.
 
---
 
## 5. Rendering — Unified 2D/2.5D/3D Abstraction
 
**RHI (Render Hardware Interface)** — single API surface, swappable backends:
 
| Platform | Backend | License |
|---|---|---|
| Linux/Windows | Vulkan 1.2 (via volk loader) | Apache 2.0/MIT (volk is MIT) |
| macOS/iOS | Metal (native, no dep) | N/A (system framework) |
| Android | Vulkan / GLES 3.2 fallback | N/A (system) |
| Embedded | Software rasterizer (custom, tile-based) | your own code |
 
- 2D: batched sprite/quad renderer, signed-distance-field text (stb_truetype, MIT)
- 2.5D: layered depth-sorted sprites + tilted-camera 3D primitives (your GameHope/WARDEN style)
- 3D: forward+ clustered renderer, PBR material model
- Shader pipeline: single HLSL-like source compiled via **glslang + SPIRV-Cross** (both MIT-compatible/BSD) to Vulkan SPIR-V, MSL (Metal), and GLSL as needed
---
 
## 6. MIT-Only Dependency Shortlist (verified license-safe)
 
| Purpose | Library | License |
|---|---|---|
| Windowing/input | SDL3 (or GLFW) | zlib / MIT — **use GLFW for strict MIT** |
| Vulkan loader | volk | MIT |
| Shader cross-compile | SPIRV-Cross | Apache 2.0 (MIT-compatible, permissive) |
| Font rendering | stb_truetype, stb_image | MIT/Public domain |
| Audio decode | dr_libs (dr_wav/dr_mp3/dr_flac) | MIT/Public domain |
| Physics (3D) | your own (Jolt is MIT if you want a reference, but you build custom per your "from scratch" requirement) | — |
| WASM plugin runtime | wasm3 | MIT |
| Networking | your own QUIC via msquic (MIT) or custom UDP+reliability | MIT |
| Local LLM runtime | llama.cpp | MIT |
| JSON/RON config | your own minimal parser (avoid heavier deps) | — |
 
Every third-party inclusion gets a `THIRD_PARTY_LICENSES.md` entry with SPDX tag — CI fails the build if a non-MIT/permissive license is introduced.
 
---
 
## 7. Plugin & Addon System
 
Two tiers, matching your GameHope two-tier isolation philosophy:
 
**Tier 1 — Native Plugins (trusted)**
- Stable C ABI (`genesis_plugin_abi_v1.h`), versioned vtable struct
- Dynamic load: `.so`/`.dll`/`.dylib`, discovered from `plugins/` manifest (`plugin.json`)
- Lifecycle: `on_load → on_register_systems → on_tick → on_unload`, dispatched through `genesis-rx` event bus
**Tier 2 — Addon Scripts (sandboxed)**
- WASM modules (wasm3), capability-scoped (no raw filesystem/network unless explicitly granted per-manifest)
- Target: marketplace-style addons, safe for third-party distribution
- Same Dart-facing-API-parity idea from GameHope applied here: WASM addons see an identical API surface regardless of native/sandboxed tier
---
 
## 8. AI Model Integration (`genesis-ai-core`)
 
- Model router: local Ollama (`localhost:11434`) or embedded `llama-server` subprocess, no cloud keys — mirrors your NAT fork setup
- Use cases wired via reactive bus:
  - **Procedural content**: terrain/dialogue/quest generation streamed as `rx_observable` tokens into the editor
  - **NPC behavior**: lightweight local model driving utility-AI decision trees (not full LLM-per-frame — LLM proposes behavior trees, engine executes them natively for performance)
  - **Vision-assisted editor**: screen-capture + local vision model for asset tagging (same pattern as FORGE Coach's screen-vision pipeline)
  - **Codegen bridge**: reuse your `CodeExportBridge` concept — engine scene graph → exportable Lua/WASM script stubs
Critical-path game logic (physics, render, input) never blocks on LLM latency — same rule you already enforced in FORGE Coach (rule-based alerts stay latency-free, LLM only for conversational/creative layer).
 
---
 
## 9. Application-Build Support (non-game apps)
 
`genesis-ui` retained-mode toolkit doubles as a general app framework:
- Widget tree with the same ECS-backed diffing as the game UI layer
- `genesis-cook` can package a UI-only project (no render/physics modules linked) → lightweight app binary
- Enables you to reuse this engine for tools like Sketchy/FlowBot-style utility apps without pulling in the full 3D pipeline (link-time module stripping via CMake feature flags)
---
 
## 10. Platform Targets & Embedded Strategy
 
| Platform | Approach |
|---|---|
| Linux/Windows/macOS | Full engine, Vulkan/Metal |
| Android | NDK + Vulkan/GLES, JNI shim (Flutter FFI bridge available for your existing Dart projects) |
| iOS | Metal, Swift/ObjC shim for lifecycle only |
| Embedded (RP2040/ESP32-class) | `genesis-embedded` profile: software rasterizer, no ECS archetype overhead — flat struct-of-arrays, static memory pools, mirrors your FORGE AI Pico ambitions |
 
---
 
## 11. Phased Roadmap (mirrors your 16-phase / 20-week precedent)
 
**Phase 0 (Week 1–2): Foundations**
- Repo scaffold, CMake+Ninja multi-toolchain build, `genesis-pal` (window/input/time/fs), `genesis-alloc`, license-audit CI script
**Phase 1 (Week 3–5): Reactive Core + Math**
- `genesis-rx` full implementation + unit tests (headless)
- `genesis-math` (vec2/3/4, mat4, quat) scalar-correct, SIMD deferred
**Phase 2 (Week 6–9): Render Backend (2D first)**
- Vulkan RHI minimal triangle → batched sprite renderer → SDF text
- Validate on Linux + Windows
**Phase 3 (Week 10–13): ECS + Scene Graph**
- Archetype-based ECS (`genesis-scene`), hot-reload via RON/JSON, editor scene tree binding through `genesis-rx`
**Phase 4 (Week 14–17): 2.5D/3D Extension**
- Depth-sorted 2.5D layer, forward+ 3D pipeline, PBR materials, camera rigs
**Phase 5 (Week 18–20): Physics + Audio**
- Custom 2D/3D physics (broadphase BVH, narrowphase SAT/GJK), dr_libs audio mixer
**Phase 6 (Week 21–23): Input + UI Toolkit**
- `genesis-ui` immediate+retained hybrid, app-mode packaging
**Phase 7 (Week 24–26): Networking**
- QUIC client/server, authoritative server model (reuse WARDEN's crate boundaries as C module boundaries)
**Phase 8 (Week 27–29): Plugin Host**
- Native ABI loader, WASM sandboxed addon runtime, plugin manifest spec + marketplace scaffold
**Phase 9 (Week 30–32): AI Bridge**
- Ollama/llama.cpp router, streaming token bus, NPC behavior-tree generator, vision-assisted asset tagging
**Phase 10 (Week 33–35): Platform Ports**
- Android NDK build, iOS Metal build, embedded profile for RP2040/ESP32
**Phase 11 (Week 36–38): Editor Shell**
- Oscilloscope/lime themed editor (reuse LocalVsCodeModel Design Studio patterns), Factory-Flow-style node graph for shader/behavior authoring
**Phase 12 (Week 39–40): Profiling & SIMD Pass**
- AVX2/NEON kernels only where profiler confirms hotspots (math, particle systems, physics broadphase)
Each phase closes with a tagged GitHub release + independently runnable headless test suite, per your standing delivery convention.
 
---
 
## 12. Open Decisions Before Implementation Start
 
1. **Windowing lib**: GLFW (strict MIT) vs SDL3 (zlib, still permissive but not MIT) — confirm your license bar (MIT-only vs "permissive-only").
2. **Physics**: fully custom from scratch (matches your stated requirement) vs referencing Jolt (MIT) for validation only, never shipping it.
3. **Scripting for gameplay logic**: Lua (MIT, tiny, battle-tested) vs WASM-only — recommend Lua for hot gameplay scripts, WASM reserved for sandboxed third-party addons.
4. **First platform to bring fully green**: recommend Linux (your primary dev machine) → then Android (Flutter FFI reuse potential with `aiin`/GameHope).
Let me know your call on these four, and I'll start Phase 0 scaffolding in the next session.
