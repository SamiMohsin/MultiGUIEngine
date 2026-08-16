# UI/UX Creativity "Godmode" Standards

Applies to any work touching `mg-ui`, `mg-editor`, example apps, or theme/asset files. The goal: never ship generic, templated-looking UI. Every screen should read as deliberately designed.

## 1. Visual identity anchor

Default engine/editor theme (can be overridden per-project, but this is the baseline "house style"):
- Background: `#0d0f14` (near-black)
- Primary accent: `#c8f135` (lime)
- Secondary accent: `#7c5cff` (violet)
- Typography: JetBrains Mono (code/technical), Outfit (UI labels/headings)
- Aesthetic direction: dark "oscilloscope" — thin glowing lines, subtle scanline/grid textures, precise technical feel rather than soft/rounded consumer-app feel

## 2. Godmode rules — before proposing any UI, the agent must:

1. **Reject default-looking output.** No unstyled system-default buttons, no Bootstrap-lookalike cards, no centered-hero-with-big-rounded-button templates unless explicitly requested. If `mg-ui`'s theme system is available, always route through it.
2. **Establish a clear visual hierarchy** — one dominant focal element per screen, secondary elements visibly secondary (size, weight, or contrast — not just color alone, for accessibility).
3. **Design for the constraint solver, not against it.** Use `mg-ui`'s flexbox-inspired layout (row/column, grow/shrink, gap, padding) as the actual construction method — do not hardcode absolute pixel positions except inside the editor's free-form canvas mode.
4. **Motion and feedback are part of the design, not an afterthought.** Any interactive element (button, toggle, slider) needs at minimum a hover/press state defined in the theme, and should be wired through `mg-rx` state changes rather than direct mutation, so transitions can be animated later without rearchitecting.
5. **Respect contrast and legibility.** Lime-on-near-black and violet-on-near-black must meet at least WCAG AA contrast for text use; if an accent color fails contrast for text, use it for borders/accents/icons only, not body text.
6. **Every new screen/panel gets a one-paragraph design rationale** in the PR description: what's the focal point, what's the reading order, why this layout over alternatives. This is not optional decoration — it's how the next agent (possibly a different model) understands intent instead of just copying pixels.

## 3. Node-graph / canvas tooling specifics (editor, shader graph, behavior graph)

- Nodes must be visually distinguishable by category via color-coding (consistent palette documented in `docs/adr/` once chosen), not just by label text.
- Connection lines should use bezier curves, not straight lines, for readability at scale.
- Grid/snap-to-grid must be on by default in any node-graph canvas.

## 4. What "creativity" does NOT mean here

- It does not mean deviating from the layer architecture or reactive-core rules to "make something cooler."
- It does not mean introducing a new UI framework or dependency — everything is built on `mg-ui` per Section 01/02 rules.
- It does not mean skipping accessibility or performance (UI still must not allocate in per-frame paths — reuse `mg-ui`'s retained widget tree, don't rebuild it every frame).

## 5. Self-check before finalizing any UI contribution

- [ ] Does this look deliberately designed, not default?
- [ ] Is there one clear focal point per screen?
- [ ] Does it use the theme system, not hardcoded colors/fonts?
- [ ] Does it pass contrast checks for any text?
- [ ] Is state wired through `mg-rx`, not direct mutation?
- [ ] Is there a one-paragraph design rationale in the PR?
