# V2I - Barrier Communication (Phase 1 MVP) — Design & Concept

## Purpose

Implement the vehicle-to-infrastructure workflow for barrier interaction.
This is the first deliverable: car approaches a barrier, barrier opens, state is tracked.

## Use Case

1. Vehicle detects proximity to a barrier (simulated threshold for MVP, real GPS/range later).
2. Vehicle-side client calls the barrier service directly.
3. Barrier service receives request and transitions `closed` → `open`.
4. Barrier service returns state to the client.
5. Vehicle publishes the state to KUKSA/VSS for cluster display.
6. **Fail-safe**: if no response within timeout (e.g., 2s), barrier stays closed, event logged.

## Vehicle Decision Rule (Barrier Use Case)

- Barrier `open` -> vehicle command: `advance`
- Barrier `closed` -> vehicle command: `stop`
- Barrier `unknown/timeout` -> vehicle command: `stop` (safety fallback)

This use case is independent from the traffic-light use case.

## Timeout & Fail-Safe Policy

- Vehicle sends request, starts timer (default: 2000ms).
- If status received before timeout expires: update VSS, reset timer.
- If timeout: set VSS to `timeout`, log event, barrier assumed **closed** (safe default).
- Retry logic: configurable (e.g., 3 retries with exponential backoff).

## Local Contract

| Signal / Call | Direction | Payload |
|---------------|-----------|---------|
| `request_barrier_open(id)` | Vehicle → Infra | `{"action": "open"}` |
| `barrier_status(id)` | Infra → Vehicle | `{"state": "open\|closed", "timestamp": "<ISO>"}` |

## Phase Progression

**Phase 1a** (MVP Sim): Pure Python simulation, all logic proven.
**Phase 1b** (Kitronik Real): Replace `barrier_simulator.py` with GPIO/serial code for Kitronik board.

---

**Implementation**: [See Technical Setup](../../src/mobility_scenarios/v2i/)
