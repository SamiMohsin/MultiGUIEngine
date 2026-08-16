# ADR 0002: Networking Transport Architecture

## Context
MultiGUIEngine requires a fast, low-latency, cross-platform networking layer (`mg-net`) for real-time multiplayer state synchronization, client-side prediction, and server reconciliation. The two primary candidates under consideration are:
1. A lightweight custom UDP protocol with an engine-native reliability and packet fragmentation layer.
2. Microsoft's `msquic` (MIT-licensed QUIC implementation).

## Decision
We adopt a **modular transport architecture** with a **lightweight custom UDP protocol as the default baseline**:
- **Default Baseline:** Custom UDP transport implementing a lightweight packet sequencing, ACK/NACK bitfield, round-trip-time (RTT) calculation, and selective packet retransmission layer. This keeps binary footprint minimal (<50KB), has zero external dependencies, and supports embedded targets.
- **Pluggable Backend:** The `mg-net` RHI/transport interface will allow `msquic` (or WebRTC data channels for Web/WASM targets) to be plugged in as an alternative backend where TLS encryption, stream multiplexing, or strict NAT traversal is required.

## Consequences
- Fast bring-up without heavy third-party build complexities.
- Compatible with low-spec/embedded hardware.
- Complete control over packet serialization and bandwidth optimization for game state snapshots.
