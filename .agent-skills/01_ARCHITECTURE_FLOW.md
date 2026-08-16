# Strict Architecture Flow Rules

## 1. Layer stack — dependency direction is one-way, always

```
L6  App/Game Layer
L5  Plugin & Addon Host   (mg-plugin-abi, mg-wasm-host)
L4  AI Bridge             (mg-ai-core, mg-ai-tools)
L3  Engine Modules        (mg-render, mg-scene, mg-physics, mg-audio, mg-ui, mg-input, mg-net, mg-asset)
L2  Reactive Core         (mg-rx)
L1  Platform Abstraction  (mg-pal, mg-math, mg-alloc)
```

**Rule:** code in layer N may `#include` and link against layer N-1 and below only. Never sideways (e.g. `mg-physics` including `mg-render` headers) and never upward (e.g. `mg-pal` including `mg-scene` headers).

**Enforcement:** before proposing any change, the agent must trace every new `#include` it adds and verify it points only downward in this stack. If a module genuinely needs to talk to a lateral module, the correct path is: emit an event via `mg-rx` and let the other module subscribe — never a direct call or header include.

## 2. Module boundary checklist (run before writing code in any module)

- [ ] Which layer is this module in?
- [ ] What am I including? Is every include from this layer or below?
- [ ] Does this module need to notify another module of something? → use `mg-rx`, not a direct call.
- [ ] Am I adding a new public function? Does it belong in this module, or does it leak a concern that belongs elsewhere (e.g. rendering logic inside `mg-scene`)?

## 3. Repository layout is fixed

Do not invent new top-level directories. The structure is defined in `docs/ARCHITECTURE.md` Section 4. If a new module is genuinely needed, propose it via an ADR (`docs/adr/NNNN-new-module-name.md`) before creating its folder.

## 4. Dependency / license process

Before adding **any** third-party code:
1. Confirm the license is MIT, BSD, zlib, Apache-2.0, or Public Domain. Reject anything else outright — do not ask "is this okay," the answer is no.
2. Add a row to `THIRD_PARTY_LICENSES.md`: library name, license, SPDX identifier, one-sentence justification, what module uses it.
3. Vendor it under `third_party/<name>/` as a pinned version (git submodule or vendored snapshot), never a floating "latest."
4. If it's not already in the approved list in `docs/ARCHITECTURE.md` Section 5, flag this explicitly in the PR description as "NEW DEPENDENCY — needs human sign-off" and do not merge without that sign-off.

## 5. Hot-path allocation rule

Any function reachable from a per-frame ECS stage (`INPUT`, `FIXED_UPDATE`, `UPDATE`, `LATE_UPDATE`, `RENDER_EXTRACT`, `RENDER`) must not call `malloc`/`free`/`new`/`delete` or any allocator other than `mg_arena_*`/`mg_pool_*` from `mg-alloc`. Before finalizing any function touching these stages, the agent must grep its own diff for allocation calls and justify or remove each one.

## 6. Correctness-before-optimization gate

No SIMD intrinsics or hand-written assembly may be introduced unless:
- A scalar/reference implementation already exists, is tested, and is kept as the fallback path, AND
- A profiling result exists under `docs/profiling/` showing this specific function as a measured bottleneck.

An agent proposing "let's optimize this with AVX2" without both of the above should instead write the scalar version and stop.

## 7. Phase discipline

Phases are defined in `docs/ARCHITECTURE.md` Section 11 and must be executed **in order**. An agent must not start Phase N+1 work while Phase N's "Definition of Done" checklist (Section 12 of the spec) is incomplete, unless the memory log explicitly records that Phase N was intentionally deferred and why.
