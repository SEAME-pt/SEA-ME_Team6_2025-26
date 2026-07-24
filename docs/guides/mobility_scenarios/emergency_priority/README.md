# Emergency Priority — Design & Concept

## Purpose

Implement emergency vehicle priority: when an emergency event is triggered,
traffic lights turn green and barriers open automatically along the route.

## Use Case

**Phase 2 (Traffic Lights)**:
1. Emergency event triggered (manual flag or sensor input in vehicle).
2. Vehicle calls the traffic-light service directly.
3. Traffic light service transitions `RED` → `GREEN`.
4. Traffic light service returns the status.
5. Vehicle publishes the status to KUKSA/VSS.

**Phase 3 (Unified V2I + Emergency)**:
- Coordinator merges barrier (Phase 1) + traffic light (Phase 2).
- Emergency overrides normal V2I flow across all nodes.
- Full orchestration: barriers open + lights green + normal traffic paused.

## Vehicle Decision Rule (Traffic-Light Use Case)

- Light `green` or `emergency_green` -> vehicle command: `advance`
- Light `yellow` -> vehicle command: `slow_down` (prepare to stop)
- Light `red` -> vehicle command: `stop`
- Light `unknown/timeout` -> vehicle command: `stop` (safety fallback)

This use case is independent from the barrier use case.

## Real Hardware Status (micro:bit + Kitronik)

- Serial communication validated over `/dev/ttyACM0`.
- Board polarity validated as `MODE HIGH` for this setup.
- Startup default remains `RED` (safe-by-default).
- Runtime bridge now maps real light state to vehicle command:
	- `red` -> `stop`
	- `yellow` -> `slow_down`
	- `green` / `emergency_green` -> `advance`
- Runtime bridge also publishes traffic-light sign class to ADAS Manager via
	`/tmp/adas_objects.sock`, enabling autonomous throttle reactions in the
	existing ADAS pipeline.
- For safety, if yellow persists beyond configured timeout, the bridge
	escalates to red (stop command path).

Implementation path:
- `src/mobility_scenarios_src/emergency_priority/microbit_trafficlight_firmware.py`
- `src/mobility_scenarios_src/emergency_priority/trafficlight_vehicle_bridge.py`

## Topics & Payload Contract

| Topic | Direction | Payload |
|-------|-----------|---------|
| `request_emergency_priority()` | Vehicle → Infra | `{"src": "ambulance\|police\|fire", "dst_zone": "zone_1"}` |
| `emergency_priority_status()` | Infra → Vehicle | `{"state": "active\|inactive", "route_status": "open\|failed"}` |
| `traffic_light_status()` | Infra → Vehicle | `{"state": "green\|red\|emergency_green"}` |

## Conflict Resolution Policy

- **Normal traffic + Emergency in same zone**: Emergency wins, barriers open, lights green.
- **Multiple emergencies**: Priority by type (fire > ambulance > police) or FIFO.
- **Timeout**: If coordinator doesn't confirm within 3s, fall back to normal V2I.

## Phase Progression

**Phase 2a** (MVP Sim): Traffic light simulator.
**Phase 2b** (Integration): Barrier + traffic light together in tests.
**Phase 3a** (Unified Sim): Coordinator service orchestrates both.
**Phase 3b** (Real Hardware): Adapt simulators to physical lights/barriers.

---

**Implementation**: [See Technical Setup](../../../src/mobility_scenarios_src/emergency_priority/)
