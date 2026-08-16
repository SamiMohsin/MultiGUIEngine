# Full Code Analysis Protocol

Every agent runs this checklist before proposing any change, regardless of size. This is deliberately mechanical so it works identically whether the agent is Claude, Gemini, a local Qwen/Mistral model, or anything else.

## Step 1 — Orient
1. Read `05_MEMORY_LOG.md`'s latest entry. What was last done, what's marked as next?
2. Read the relevant section(s) of `docs/ARCHITECTURE.md` for the module(s) you're touching.
3. Read any `docs/adr/` entries referencing this module.
4. `git log --oneline -20` on the relevant module path to see recent real history (memory log is the intent record, git log is the ground truth — if they disagree, trust git log and flag the discrepancy).

## Step 2 — Scope the change
1. State explicitly, before writing code: "This change touches modules: [list]." If the list includes more than 2 modules, stop and reconsider — most tasks should be single-module.
2. Verify against `01_ARCHITECTURE_FLOW.md` that every included module is a valid downward dependency from every other.

## Step 3 — Write / modify code
1. Follow `docs/CODING_STANDARDS.md` (C99, `mg_` prefix, no hot-path malloc, doc comments on public functions).
2. Follow `02_REACTIVE_LIBRARY_RULES.md` for any cross-module signaling.
3. Follow `03_UIUX_CREATIVITY_GODMODE.md` if touching UI.

## Step 4 — Self-review diff (mandatory, do this even if confident)
Go through your own diff line by line and check for:
- [ ] Any upward or lateral `#include` violating the layer stack
- [ ] Any `malloc`/`free`/`new`/`delete` reachable from a per-frame stage
- [ ] Any new third-party dependency not logged in `THIRD_PARTY_LICENSES.md`
- [ ] Any public function missing a doc comment
- [ ] Any `subscribe` without a matching `dispose`
- [ ] Any hardcoded platform-specific code outside `mg-pal`'s per-platform folders
- [ ] Any magic numbers that should be named constants or config values
- [ ] Any TODO/FIXME left without a corresponding note in the memory log

## Step 5 — Test
1. Build headless (`MG_HEADLESS_TESTS=ON`) for the touched module(s).
2. Run the module's unit test suite. If you cannot actually execute the build (no compiler available in your environment), say so explicitly in the PR description — never claim tests passed if they weren't run.
3. If this touches rendering, diff output against the software backend reference where applicable.

## Step 6 — Impact analysis
1. What else in the repo calls the functions you changed? Grep for call sites, not just your mental model of them.
2. Does this change any public API signature? If yes, every call site must be updated in the same PR — no partial migrations left for "later."
3. Does this affect any other platform's build (Windows/macOS/Android/iOS/embedded) even if you only tested Linux? Flag explicitly if you can't verify cross-platform impact.

## Step 7 — Record
1. Update `05_MEMORY_LOG.md` per `06_COMMIT_RULES.md`'s template — what you did, what you verified, what's next, what model you are.
2. If any architectural decision was made or changed, write or update an ADR in `docs/adr/`.

## Step 8 — Commit
Follow `06_COMMIT_RULES.md` exactly for commit message format and PR structure.

---

**This protocol is not optional for small changes.** A one-line fix still needs Steps 1, 4, 5, 7, 8 at minimum. Skipping steps is how architecture drifts across a multi-model contributor history — the whole point of this file is that no single agent has to be trusted to "just remember" the rules; the protocol enforces it every time.
