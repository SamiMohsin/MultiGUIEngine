# MultiGUIEngine

A from-scratch, cross-platform 2D/2.5D/3D game and application engine written in C (with Assembly for profiler-confirmed hot paths), using only MIT/permissive-licensed dependencies. Built on a reactive event core, with a native + WASM plugin/addon system, and optional local AI model integration (Ollama / llama.cpp — no cloud keys).

- **License:** MIT
- **Platforms:** Linux, Windows, macOS, Android, iOS, Embedded (RP2040/ESP32-class)
- **Status:** Phase 0 — repo scaffold + `mg-pal` stub building and testing green

## Getting started (Phase 0)

```bash
mkdir build && cd build
cmake -G Ninja ..
ninja
ctest --output-on-failure
```

## Where to look

| I want to... | Go to |
|---|---|
| Understand the full architecture, module APIs, and build order | [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) |
| Contribute as (or with) an AI agent | [`AGENTS.md`](AGENTS.md) → [`.agent-skills/`](.agent-skills/) |
| See what's already been built and what's next | [`.agent-skills/05_MEMORY_LOG.md`](.agent-skills/05_MEMORY_LOG.md) |
| Check dependency licenses | [`THIRD_PARTY_LICENSES.md`](THIRD_PARTY_LICENSES.md) |
| See a past architectural decision | [`docs/adr/`](docs/adr/) |

## Build order

The engine is built in 16 strict phases (Phase 0 = this scaffold, Phase 16 = profiling/SIMD pass). See `docs/ARCHITECTURE.md` Section 11 for the full list. Do not skip ahead — each phase's Definition of Done (Section 12) must be satisfied before the next begins.
