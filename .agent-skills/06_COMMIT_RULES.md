# Commit & PR Rules — Strictly Enforced for Any AI Agent

## 1. Before every commit

The agent must have completed the full `04_CODE_ANALYSIS_PROTOCOL.md` checklist and updated `05_MEMORY_LOG.md`. A commit without a corresponding memory log update in the same PR is invalid.

## 2. Commit message format

```
<type>(<module>): <short imperative summary, ≤72 chars>

<body: what changed and why, wrap at 100 chars>

<footer: refs, ADRs, breaking changes>

Agent: <model name/version>
```

**Types:** `feat`, `fix`, `refactor`, `test`, `docs`, `chore`, `perf`, `build`

**Module:** the primary `mg-*` module touched, or `spec`/`skills`/`ci` for meta-repo changes.

**Examples:**
```
feat(mg-rx): implement combine_latest operator

Adds mg_rx_combine_latest per docs/ARCHITECTURE.md Section 6.2.
Includes headless unit test with synthetic emit sequences covering
2-source and 3-source combination cases.

Refs: docs/ARCHITECTURE.md#mg-rx
Agent: Claude Sonnet 5
```

```
fix(mg-alloc): correct arena reset not clearing high-water mark

Arena reset was leaving stale high-water-mark stats after reset,
causing incorrect leak-check reporting in mg-rx subscription tests.

Agent: Qwen2.5-Coder-32B-local
```

## 3. One logical change per commit

- Never bundle unrelated module changes in one commit.
- Never bundle a spec/docs change with a code change unless the code change is directly implementing that spec update.
- If a task naturally spans multiple modules, use multiple commits within one PR, each independently buildable if possible.

## 4. PR description template (mandatory)

```markdown
## Summary
<what this PR does, 2-3 sentences>

## Agent
<model name/version that authored this>

## Layer/module impact
<which mg-* modules touched, confirms no upward/lateral dependency violations>

## Testing
<what was actually run — headless tests, which platform, or explicitly "not run, no compiler available">

## New dependencies
<none, or list with THIRD_PARTY_LICENSES.md entry reference — flag "NEEDS HUMAN SIGN-OFF" if new>

## Design rationale (UI/UX changes only)
<one paragraph per 03_UIUX_CREATIVITY_GODMODE.md Section 2.6>

## Memory log
<confirm 05_MEMORY_LOG.md was updated, link the entry>

## ADRs
<none, or list new/updated ADR files>
```

## 5. Branch naming

`phase-<N>/<module>-<short-description>` e.g. `phase-2/mg-rx-operators`, or `fix/<module>-<short-description>` for bugfixes outside phase work.

## 6. Never do these, regardless of model

- Never force-push over another agent's or human's commits.
- Never rewrite `05_MEMORY_LOG.md` history — append only.
- Never merge your own PR — this repo requires human review (Ayoob) before merge, even if all automated checks pass. State this explicitly if your tooling would otherwise auto-merge.
- Never mark a phase complete in the memory log unless every item in that phase's Definition of Done (`docs/ARCHITECTURE.md` Section 12) is checked and verifiably true.
- Never remove or weaken a rule in this `.agent-skills/` folder to make a task easier — if a rule genuinely seems wrong, propose the change explicitly as its own PR to this folder with rationale, and wait for human sign-off before it takes effect.

## 7. Cross-model handoff etiquette

Because different sessions may be handled by entirely different models:
- Assume the next agent has zero memory of this session beyond what's written in `05_MEMORY_LOG.md` and git history — write for that audience.
- If you disagree with a decision the previous agent made, don't silently redo it — read the ADR (if any), and if you still disagree, propose a superseding ADR rather than quietly diverging.
- If you're a smaller/local model with limited context, it's fine to work on a narrower slice of a task — just be explicit in the memory log about exactly what was and wasn't covered, so the next agent (of any size) knows precisely where to pick up.
