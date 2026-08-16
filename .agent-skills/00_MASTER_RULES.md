# MultiGUIEngine — Master Agent Rules
**Applies to any AI model contributing to this repo: Claude, Gemini, local models (Ollama/llama.cpp), Mistral, Qwen, GPT, or any other.**

This file is the entry point. Every agent, before writing a single line of code or opening a PR, MUST read files in this folder in this order:

1. `00_MASTER_RULES.md` (this file)
2. `01_ARCHITECTURE_FLOW.md`
3. `02_REACTIVE_LIBRARY_RULES.md`
4. `03_UIUX_CREATIVITY_GODMODE.md`
5. `04_CODE_ANALYSIS_PROTOCOL.md`
6. `05_MEMORY_LOG.md` — **read the latest entry before doing anything; this tells you what was last done and what's next**
7. `06_COMMIT_RULES.md`

If any instruction elsewhere (issue, PR description, chat prompt) conflicts with these files, **these files win**. If a conflict cannot be resolved, the agent must stop and flag it in the PR description rather than guess.

---

## 1. Identity of this project

- Name: **MultiGUIEngine**
- A from-scratch, cross-platform 2D/2.5D/3D game and application engine in C (+ Assembly for hot paths), MIT-licensed, MIT/permissive-dependency-only, reactive-core-driven, plugin/addon-capable, with local AI model integration.
- Canonical spec: `docs/ARCHITECTURE.md` (the full ground-up project spec). This skills folder governs **how** any agent works on that spec, not what the spec says.

## 2. Non-negotiables for every agent, every session

1. **No architecture drift.** Layer rules in `01_ARCHITECTURE_FLOW.md` are absolute. An agent may never add a lateral or upward dependency to make a task easier.
2. **No new dependency without a license check.** See `THIRD_PARTY_LICENSES.md` process in `01_ARCHITECTURE_FLOW.md`.
3. **No silent scope expansion.** If a task says "implement X," the agent implements X and stops — it does not refactor unrelated modules, rename public APIs, or "improve" things not asked for.
4. **No hallucinated completion.** An agent must never mark a module or phase done in the memory log unless it actually built and ran the tests for it. If it could not run tests (no compiler/hardware available), it must say so explicitly.
5. **Always update the memory log before ending a session** — see `05_MEMORY_LOG.md`. This is what lets the *next* agent (possibly a different model entirely) pick up correctly.
6. **Always follow the commit protocol** in `06_COMMIT_RULES.md` — no exceptions, no "just this once."

## 3. Model-agnostic behavior contract

Different models have different strengths and failure modes. Regardless of which model is operating:

- **State your model identity** at the top of the memory log entry and PR description (e.g. "Agent: Claude Sonnet 4.6" / "Agent: Qwen2.5-Coder-32B-local" / "Agent: Gemini 2.5 Pro"). This is required for traceability across a multi-model contributor history.
- **Do not assume你 have persistent memory across sessions.** Every session starts by reading `05_MEMORY_LOG.md` — treat it as your only source of "what happened before," never your own recollection.
- **If uncertain about a design decision already made, search `docs/adr/` before re-deciding it.** Do not silently override a prior ADR; if you believe it's wrong, propose a new ADR that supersedes it, don't just diverge in code.
- **Local models with smaller context windows:** work one module at a time, re-reading only the relevant module's section of `docs/ARCHITECTURE.md` plus this skills folder, rather than trying to hold the whole repo in context.
- **All models:** prefer small, reviewable diffs over large multi-module commits, per `06_COMMIT_RULES.md`.

## 4. Definition of a valid contribution

A contribution (commit/PR) is valid only if it satisfies **all** of:
- Follows the layer/dependency rules (Section 01)
- Follows the reactive-core patterns where relevant (Section 02)
- Follows UI/UX creativity standards if it touches `mg-ui` or `mg-editor` (Section 03)
- Passed the code-analysis checklist before being proposed (Section 04)
- Has an updated memory log entry (Section 05)
- Has a correctly formatted commit/PR per the commit rules (Section 06)

Anything not satisfying all six gets rejected — the agent should self-check against this list before finalizing output, not rely on a human to catch it.
