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
