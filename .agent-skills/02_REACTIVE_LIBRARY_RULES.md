# Reactive Core (`mg-rx`) — Build & Usage Rules

`mg-rx` is the spine connecting every module. Getting this wrong breaks the entire architecture, so it has its own strict ruleset.

## 1. Core types agents must use exactly as named

- `mg_subject_t` — hot observable, no replay
- `mg_behavior_subject_t` — holds last value, replays it to new subscribers immediately on subscribe
- `mg_observable_t` — result of an operator chain, read-only from the consumer's side
- `mg_subscription_t` — returned from every `subscribe` call, must be disposed exactly once

Do not invent parallel event mechanisms (custom callback lists, global function pointers, ad-hoc queues) anywhere in the engine. If something needs to notify other code of an event, it goes through `mg-rx`.

## 2. Operators — implement only what's specified, keep signatures consistent

`mg_rx_map`, `mg_rx_filter`, `mg_rx_debounce_ms`, `mg_rx_throttle_ms`, `mg_rx_merge`, `mg_rx_combine_latest`, `mg_rx_scan`

Every operator:
- Takes a source `mg_observable_t*` (or subject) and returns a new `mg_observable_t*`
- Never blocks the calling thread
- Never allocates from the hot-path frame allocator — operator state lives in its own pool allocated at subscribe-time and freed at dispose-time

## 3. Threading contract

- Producer threads (input, net, asset watcher, audio) never call directly into main-thread-owned modules.
- Producers push onto a lock-free SPSC `mg_ring_buffer_t` (from `mg-alloc`).
- The main loop drains all ring buffers **once, at a fixed point per frame** (start of the `INPUT` stage) and re-emits as `mg-rx` events on the main-thread bus.
- No module may subscribe to a cross-thread subject directly without going through this drain step. Any agent adding a new producer thread must wire it through this exact pattern — no exceptions, no "just this once it's simpler to call directly."

## 4. Subscription lifecycle discipline

- Every `subscribe` call's returned `mg_subscription_t*` must be stored by the owner and disposed in the owning system's teardown/`on_unload`.
- Never subscribe inside a per-frame function (this leaks a new subscription every frame). Subscriptions are set up once, typically in a module's `on_register_systems` or init function.
- Before finalizing any change that adds a `subscribe` call, the agent must locate the matching `dispose` call in the same diff or explain why none is needed (e.g. subscription is intentionally process-lifetime).

## 5. Testing requirement

Every new operator or subject usage pattern needs a headless unit test that:
- Emits a synthetic sequence of values
- Asserts the exact expected output sequence (order and content)
- Asserts subscriptions dispose cleanly with no leaked memory (verify via the arena/pool allocator's own leak-check, not an external tool)

## 6. Anti-patterns to reject on sight

- A module polling another module's state every frame instead of subscribing to its changes.
- A subject that's emitted to from multiple threads without going through the ring-buffer drain pattern.
- Using `mg-rx` for high-frequency per-vertex or per-pixel data — that's what the render/asset pipelines are for. `mg-rx` is for state changes and discrete events, not bulk data streaming.
