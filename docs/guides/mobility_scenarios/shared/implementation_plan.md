
# Module 3 Implementation Plan (V2I + Emergency Priority)

## 1. Objectives

- Enable controlled interaction between vehicle and roadside barriers (Phase 1).
- Add emergency priority for traffic lights (Phase 2).
- Combine V2I + Emergency Priority into a unified scenario (Phase 3).
- Keep compatibility with current architecture: STM32 + CAN + AGL + KUKSA + Qt.

## 2. Architecture Boundaries

- No uProtocol usage.
- VSS + KUKSA Databroker are the primary signal layer.
- Existing CAN path remains authoritative for low-level vehicle state.
- MQTT is optional and only considered later if we need a brokered simulator.

## 3. Tech Stack Decision

### Language: Python

- The team already uses Python for KUKSA clients and ADAS scripts.
- KUKSA Python SDK is already used in the project ecosystem.
- Fast to prototype and iterate — ideal for MVP-first approach.

### MVP Communication Style: Direct local service calls

- The first version should avoid external broker dependency.
- The vehicle-side client can trigger the simulated infrastructure service directly.
- Barrier and traffic-light states are then mirrored to KUKSA/VSS for display.
- If we later need decoupling, MQTT can be introduced as an adapter layer.

### Architecture per scenario

```
Vehicle (Pi5/AGL)
  └── V2I Client (Python)
        ├── detects proximity / emergency trigger
        ├── calls the simulated infrastructure service directly
        └── publishes status to KUKSA/VSS

Infrastructure Simulator (Python, local process or service)
  └── Barrier Service / Traffic Light Service
        ├── receives direct calls from vehicle-side code
        └── updates internal state and status output
```

## 4. Phased Plan

### Phase 1 — V2I Barrier MVP (fast first test)

**Goal**: car detects proximity to a barrier and the barrier opens.

- Simulated barrier in Python with direct local calls.
- Vehicle-side client: detects proximity (simulated threshold or keypress for MVP).
- Vehicle triggers the barrier service directly and receives a status response.
- Barrier status published to KUKSA/VSS signal for traceability.
- Fail-safe: if no response within timeout, barrier stays closed, log event.

**Deliverable**: working demo — car approaches, barrier opens, status visible.

### Phase 2 — Emergency Vehicle Priority (traffic lights)

**Goal**: emergency event triggers green on traffic lights.

- Simulated traffic light service in Python with direct local calls.
- Emergency event source: manual trigger or simulated flag on vehicle.
- Vehicle/service triggers priority directly on the traffic light service.
- Status published to KUKSA/VSS.
- Conflict handling: if normal vehicle present, emergency gets priority.

**Deliverable**: demo — emergency trigger fires, traffic light turns green.

### Phase 3 — Unified V2I + Emergency Priority

**Goal**: single coordinator handles barriers + traffic lights, with emergency override.

- Merge barrier client and traffic light client into one coordinator service.
- Emergency flag overrides normal V2I flow across all infrastructure nodes.
- Unified KUKSA/VSS signal set for monitoring both subsystems.
- Full integration test with both normal and emergency scenarios.

**Deliverable**: integrated demo covering barriers + lights + emergency override.

## 5. Signal Contract (Proposed)

| Signal / Topic | Direction | Description |
|----------------|-----------|-------------|
| `Vehicle.Infra.Barrier.Status` | Vehicle → KUKSA | Barrier current state (open/closed) |
| `Vehicle.Infra.TrafficLight.Status` | Vehicle → KUKSA | Traffic light current state |
| `Vehicle.Emergency.Priority.Active` | Vehicle → KUKSA | Emergency priority active |

## 6. Suggested Deliverables

- Working Phase 1 demo (barrier open/close).
- Sequence diagrams for each phase.
- Signal contract table.
- Test matrix with pass/fail evidence.
- Demo script for sprint presentation.

## 7. Risks and Mitigations

- Missing real infrastructure: all services run as Python simulators first.
- Signal mismatch: lock interface table before writing code.
- KUKSA availability: keep a local stub/fallback for the first demo.

## 1. Objectives

- Enable controlled interaction between vehicle and roadside barriers (Phase 1).
- Add emergency priority for traffic lights (Phase 2).
- Combine V2I + Emergency Priority into a unified scenario (Phase 3).
- Keep compatibility with current architecture: STM32 + CAN + AGL + KUKSA + Qt.

## 2. Architecture Boundaries

- No uProtocol usage.
- VSS + KUKSA Databroker are the primary signal layer.
- Existing CAN path remains authoritative for low-level vehicle state.
- MQTT is optional and only used if we later need a lightweight simulator bridge.

## 3. Tech Stack Decision

### Language: Python

- The team already uses Python for KUKSA clients and ADAS scripts.
- KUKSA Python SDK (`kuksa-client`) is available and proven in the project.
- Fast to prototype and iterate — ideal for MVP-first approach.

### Protocol: MQTT (via `paho-mqtt`)

- Lightweight pub/sub — perfect for infrastructure simulation.
- Team already spiked MQTT (see `docs/guides/MQTT_spike.md`).
- Natural fit: infrastructure publishes state, vehicle subscribes and reacts.
- Low latency and minimal setup vs REST.

### Architecture per scenario

```
Vehicle (Pi5/AGL)
  └── V2I Client (Python)
        ├── publishes proximity/request → MQTT broker
        ├── subscribes to barrier/traffic light status ← MQTT broker
        └── publishes status to KUKSA/VSS

Infrastructure Simulator (Python, runs on same Pi5 or laptop)
  └── Barrier Service / Traffic Light Service
        ├── subscribes to vehicle requests ← MQTT broker
        └── publishes state changes → MQTT broker
```

## 4. Phased Plan

### Phase 1 — V2I Barrier MVP (fast first test)

**Goal**: car detects proximity to a barrier and the barrier opens.

- Simulated barrier (Python service subscribing/publishing via MQTT).
- Vehicle-side client: detects proximity (simulated threshold or keypress for MVP).
- Vehicle publishes `v2i/barrier/{id}/request` → barrier opens, replies on `v2i/barrier/{id}/status`.
- Barrier status published to KUKSA VSS signal for traceability.
- Fail-safe: if no response within timeout, barrier stays closed, log event.

**Deliverable**: working demo — car approaches, barrier opens, status visible.

### Phase 2 — Emergency Vehicle Priority (traffic lights)

**Goal**: emergency event triggers green on traffic lights.

- Simulated traffic light service (Python, MQTT).
- Emergency event source: manual trigger or simulated flag on vehicle.
- Vehicle/service publishes `emergency/priority/request` → traffic light replies with green status.
- Status published to KUKSA VSS.
- Conflict handling: if normal vehicle present, emergency gets priority.

**Deliverable**: demo — emergency trigger fires, traffic light turns green.

### Phase 3 — Unified V2I + Emergency Priority

**Goal**: single coordinator handles barriers + traffic lights, with emergency override.

- Merge barrier client and traffic light client into one coordinator service.
- Emergency flag overrides normal V2I flow across all infrastructure nodes.
- Unified KUKSA/VSS signal set for monitoring both subsystems.
- Full integration test with both normal and emergency scenarios.

**Deliverable**: integrated demo covering barriers + lights + emergency override.

## 5. MQTT Topic Convention (Proposed)

| Topic | Direction | Description |
|-------|-----------|-------------|
| `v2i/barrier/{id}/request` | Vehicle → Infra | Vehicle requests barrier open |
| `v2i/barrier/{id}/status` | Infra → Vehicle | Barrier current state (open/closed) |
| `v2i/trafficlight/{id}/status` | Infra → Vehicle | Traffic light current state |
| `emergency/priority/request` | Vehicle → Infra | Emergency vehicle priority request |
| `emergency/priority/status` | Infra → Vehicle | Confirmed priority state |

## 6. VSS Signals (Proposed)

| VSS Path | Type | Description |
|----------|------|-------------|
| `Vehicle.Infra.Barrier.Status` | string | open / closed / unknown |
| `Vehicle.Infra.TrafficLight.Status` | string | green / red / unknown |
| `Vehicle.Emergency.Priority.Active` | bool | emergency priority active |

## 7. Suggested Deliverables

- Working Phase 1 demo (barrier open/close).
- Sequence diagrams for each phase.
- MQTT topic + VSS signal contract table.
- Test matrix with pass/fail evidence.
- Demo script for sprint presentation.

## 8. Risks and Mitigations

- Missing real infrastructure: all services run as Python simulators first.
- Signal mismatch: lock interface table before writing code.
- MQTT broker availability: use a local Mosquitto instance on Pi5.
