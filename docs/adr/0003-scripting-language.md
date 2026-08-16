# ADR 0003: Gameplay and Addon Scripting Engine

## Context
The engine requires a mechanism for hot-reloadable gameplay logic and sandboxed user/community addons. Two options were evaluated:
1. Embedding Lua (e.g., Lua 5.4 or LuaJIT, MIT-licensed).
2. A sandboxed WebAssembly (WASM) runtime (`wasm3`, MIT-licensed).

## Decision
We select **Sandboxed WebAssembly (WASM via wasm3) as the canonical addon/scripting engine**, with an optional lightweight **Lua bridge for development/rapid prototyping**:
- **Production & Addons (`mg-wasm-host`):** WebAssembly using `wasm3` provides memory isolation, strict capability security (explicit host-function binding), multi-language authoring (C, Rust, Zig, AssemblyScript, Go), and fast interpretation across all platforms (including iOS/consoles where JIT is restricted).
- **Rapid Prototyping (`mg-plugin-lua`, optional tool):** Lua can be optionally included in debug/editor builds for interactive REPL and quick iteration, but the official addon distribution format will be WASM.

## Consequences
- Addons cannot compromise host application security or crash the host process.
- Modders can write addons in their preferred compiled language.
- Deterministic sandbox execution without complex language-specific bindings.
