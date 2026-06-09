# Planning V2I (Living Plan)

## Why this file exists

This file is the single source of truth for implementation progress of Module 3 mobility scenarios.
It must be updated as implementation evolves.

## Owner and Scope

- Owner: Joao
- Branch: `feature/mobility_scenarios/V2I_and_emergencypriority`
- Scope:
  - Phase 1: V2I barriers (MVP)
  - Phase 2: Emergency Priority (traffic lights)
  - Phase 3: Unified V2I + Emergency Priority

## Where this file should live

- This file lives in `docs/guides` because it is planning and process documentation.
- `src/` should contain runnable code, configs, and tests.

## Current Status

- [x] Branch prepared and synced
- [x] Base folders created in `docs/guides/mobility_scenarios`
- [x] Base folders created in `src/mobility_scenarios_src`
- [x] High-level implementation plan drafted
- [x] Phase 1 code scaffold created
- [x] Phase 1 MVP running end-to-end
- [x] Phase 1 evidence collected
- [x] Hardware adapter scaffold created for future Kitronik integration
- [x] Backend selection tests added
- [x] Phase 2 code scaffold created
- [x] Phase 2 MVP running end-to-end
- [x] Phase 2 evidence collected
- [x] Phase 3 unified flow implemented
- [x] Phase 3 evidence and demo script completed

## Implementation Plan

### Phase 1 - V2I Barriers MVP (fast first)

Goal: Vehicle requests barrier opening and receives status confirmation.

Tasks:
- [x] Define barrier interaction contract for local MVP
- [x] Create `barrier_simulator.py` (simulated infrastructure side)
- [x] Create `v2i_client.py` (vehicle side)
- [x] Create `config.json` with local simulation and timeout settings
- [x] Publish barrier status into KUKSA/VSS (stubbed writer with optional toggle)
- [x] Implement timeout fail-safe (default: barrier remains closed)
- [x] Add unit tests (state and timeout)
- [x] Add integration test (direct local request/response)
- [x] Capture first MVP demo evidence

Exit criteria:
- [x] Car/client sends request and receives open/closed status
- [x] Timeout path works and is logged
- [x] Status visible in KUKSA/VSS

### Phase 2 - Emergency Priority (traffic lights)

Goal: Emergency trigger forces green traffic light with confirmation.

Tasks:
- [x] Define emergency interaction contract for local MVP
- [x] Create `trafficlight_simulator.py`
- [x] Create `emergency_client.py`
- [x] Implement emergency override policy
- [x] Publish traffic light and emergency states to KUKSA/VSS (stubbed writer)
- [x] Add tests for normal/emergency conflict
- [x] Capture phase demo evidence

Exit criteria:
- [x] Emergency trigger changes traffic light state to green
- [x] State propagation confirmed locally and in KUKSA/VSS

### Phase 3 - Unified V2I + Emergency Priority

Goal: One coordinator orchestrates barriers and traffic lights with emergency override.

Tasks:
- [x] Create `coordinator.py`
- [x] Merge barrier and traffic light workflows
- [x] Implement priority policy and fallback behavior
- [x] Add end-to-end integration tests
- [x] Prepare final demo script

Exit criteria:
- [x] Unified scenario works with and without emergency mode
- [x] Test evidence ready for sprint review

## Decisions Log

- 2026-06-03: MVP-first strategy approved (simulate first, hardware later).
- 2026-06-03: Stack selected: Python + KUKSA/VSS with local simulation first; MQTT kept optional.
- 2026-06-03: Documentation split enforced:
  - `docs/guides` = design/planning/architecture
  - `src` = implementation/run/tests
- 2026-06-03: Barrier and traffic-light are independent use cases with separate vehicle motion rules.

## Update Rules

When finishing a task:
1. Mark checklist item as done.
2. Add one line in Progress Updates with date and outcome.
3. If scope changed, record it in Decisions Log.

## Progress Updates

- 2026-06-03: Created living plan file and initialized phase checklists.
- 2026-06-03: Implemented Phase 1 scaffold in `src/mobility_scenarios_src/v2i` with `barrier_simulator.py`, `v2i_client.py`, `config.json`, `requirements.txt`, and first unit tests.
- 2026-06-03: Python syntax validation passed for Phase 1 scripts (`python3 -m py_compile`).
- 2026-06-03: Pytest run was interrupted in terminal and needs re-run to confirm green test status.
- 2026-06-03: Refactored V2I MVP to broker-free local direct calls; Mosquitto is no longer required.
- 2026-06-03: Verified local end-to-end flow with a smoke test (`V2IClient` + `BarrierSimulator`).
- 2026-06-03: Renamed the runtime config to `config.json` to match the standard-library JSON loader.
- 2026-06-03: Added a backend abstraction and a Kitronik adapter placeholder for the future hardware step.
- 2026-06-03: Added backend selection tests and confirmed the placeholder Kitronik adapter returns a valid status payload.
- 2026-06-03: Implemented Emergency Priority Phase 2 scaffold (`trafficlight_simulator.py`, `emergency_client.py`, `coordinator.py`) with broker-free local flow.
- 2026-06-03: Validated Phase 2 local end-to-end emergency flow (client + traffic light simulator).
- 2026-06-03: Integrated Phase 3 unified coordinator with traffic light + barrier and validated emergency/non-emergency behavior in local smoke tests.
- 2026-06-03: Added `unified_demo.py` and completed a deterministic local demo path for sprint evidence.
- 2026-06-03: Added `docs/guides/mobility_scenarios/shared/demo_script_module3.md` with Monday hardware checklist and acceptance evidence steps.
- 2026-06-03: Added automated preflight (`src/mobility_scenarios_src/run_preflight.sh`) and validated full local readiness flow (`PASS`).
- 2026-06-08: Validated real micro:bit traffic-light serial control; board polarity confirmed as `MODE HIGH` for this setup.
- 2026-06-08: Added `trafficlight_vehicle_bridge.py` to map real light state into vehicle motion (`stop`, `slow_down`, `advance`).
- 2026-06-08: Added interactive `serial_step_by_step.py` flow to validate physical colors safely and slowly during hardware bring-up.
- 2026-06-08: Integrated bridge output with ADAS Manager via `/tmp/adas_objects.sock` so autonomous driving receives traffic-light classes directly.
- 2026-06-08: Added yellow persistence safety behavior (`yellow_stop_after_s`) to escalate from slow down to stop.
- 2026-06-09: Wireless integration plan finalized: semaforo sends state by network to AGL, bridge publishes to KUKSA/VSS and ADAS Manager.

## Wireless Architecture (Phase 2/3 Remote Deployment)

### Overview

For deployment without physical cables between the micro:bit traffic light system and the car:

```
┌──────────────────┐         [WiFi/Network]         ┌─────────────┐         [CAN Bus]         ┌─────────────┐
│  Micro:bit +     │  ─────────────────────────→  │   AGL Pi5   │  ───────────────────→  │  STM32      │
│  Kitronik Light  │                               │  (AGL/ADAS) │                        │  (Motor)    │
│                  │←─────────────────────────────│             │                        │             │
│  (Serial USB)    │   Network Bridge Receiver     │             │                        │             │
└──────────────────┘                               └─────────────┘                        └─────────────┘
                                                           │
                                                           │ KUKSA/VSS
                                                           ↓
                                                    ┌─────────────┐
                                                    │   Qt Cluster│
                                                    │   (Display) │
                                                    └─────────────┘
```

### Signal Flow

1. **Micro:bit** sends traffic-light state (`red`/`yellow`/`green`) over serial USB to **local PC** or network device.
2. **AGL Bridge Receiver** (lightweight Python service on AGL) receives the state over WiFi/network.
3. **Bridge** publishes to two destinations:
   - **KUKSA/VSS**: signal `Vehicle.Infra.TrafficLight.Status` for the cluster display.
   - **ADAS Manager**: `ObjectFrame` with `SIGN_TL_RED/YELLOW/GREEN` to `/tmp/adas_objects.sock` for autonomous driving control.
4. **ADAS Manager** reads the traffic-light class and applies throttle limit:
   - `SIGN_TL_RED` → throttle = 0 (stop)
   - `SIGN_TL_YELLOW` → throttle = 50 (slow down)
   - `SIGN_TL_GREEN` → throttle = 100 (continue)
5. **CAN** sends control command to **STM32** (motor actuation).

### Components

- **Bridge Transmitter** (on a host PC connected to the micro:bit by USB):
  - Reads serial state from micro:bit or simulator
  - Sends `{"light_state": "red|yellow|green"}` over UDP/TCP to AGL
  - Note: the current micro:bit firmware is serial-only; it does not transmit this state directly over wireless
  
- **Bridge Receiver** (on AGL):
  - Runs as daemon or systemd service
  - Receives network state from transmitter
  - Publishes to KUKSA Databroker (VSS) and ADAS object socket
  - Includes safety timeouts: if no update for 5s, defaults to `red`

- **ADAS Manager** (validated runtime folder: `/data/ADAS-Manager-OA-v7/`):
  - Already handles `SIGN_TL_RED/YELLOW/GREEN` in throttle override logic
  - No changes needed to this component

### Configuration

Example `config.json` for the bridge:

```json
{
  "traffic_light": {
    "transmitter_host": "192.168.1.XX",
    "transmitter_port": 5555,
    "timeout_sec": 5.0
  },
  "wireless": {
    "protocol": "udp",
    "listen_port": 5555,
    "heartbeat_interval_sec": 1.0
  },
  "adas_bridge": {
    "enabled": true,
    "object_socket": "/tmp/adas_objects.sock",
    "yellow_stop_after_s": 2.0
  },
  "kuksa": {
    "enabled": true,
    "databroker_host": "localhost",
    "databroker_port": 55555,
    "signal_path": "Vehicle.Infra.TrafficLight.Status"
  }
}
```

### Deployment Steps

1. **On PC (with micro:bit)**:
   ```bash
  python trafficlight_vehicle_bridge.py --bridge-mode transmitter --agl-host 10.21.220.191 --config config.json
   ```

2. **On AGL**:
   ```bash
  cd /data/ADAS-Manager-OA-v7
  python trafficlight_vehicle_bridge.py --bridge-mode receiver --config config.json
   ```
   Or install as systemd service:
   ```bash
   sudo systemctl start adas-trafficlight-bridge.service
   ```

3. **Verify**:
   - Check KUKSA/VSS: `Vehicle.Infra.TrafficLight.Status` should show `red|yellow|green`
   - Watch ADAS log: should show `*** THROTTLE OVERRIDE=...` when light changes
   - Test car motion: red → stop, yellow → slow, green → continue

### Safety

- **Default state**: `red` (safe shutdown on any bridge failure)
- **Yellow timeout**: escalates to `red` after 2 seconds to prevent indefinite "slow down" state
- **Heartbeat**: bridge receiver triggers safe stop if no update for 5 seconds
- **No single point of failure**: if bridge dies, ADAS Manager continues with old state; manual override always available

## Monday Hand-Off

### Current Checkpoint (End of Wednesday)

- Local MVP is complete for:
  - V2I barrier flow (independent use case)
  - Emergency traffic-light flow (independent use case)
  - Unified coordinator flow (emergency override)
- Local evidence is available:
  - `docs/guides/mobility_scenarios/shared/evidence/local_demo_2026-06-03.log`
  - `docs/guides/mobility_scenarios/shared/evidence/rules_validation_2026-06-03.log`
- One-command local preflight is available and validated:
  - `bash src/mobility_scenarios_src/run_preflight.sh`

### What Is Still Pending for Monday (Hardware Day)

1. Implement real Kitronik barrier control in:
  - `src/mobility_scenarios_src/v2i/kitronik_barrier.py`
2. Switch barrier backend in:
  - `src/mobility_scenarios_src/v2i/config.json`
   - set `barrier.backend = "hardware"`
3. Implement real traffic-light hardware control in:
  - `src/mobility_scenarios_src/emergency_priority/trafficlight_simulator.py`
   - replace hardware placeholder path with real GPIO logic (red/yellow/green)
4. Run hardware validation sequence:
   - Barrier closed -> vehicle `stop`
   - Barrier open -> vehicle `advance`
   - Light green -> `advance`
   - Light yellow -> `slow_down`
   - Light red -> `stop`
   - Emergency on -> `emergency_green` + barrier `open`
5. Collect hardware evidence:
   - terminal logs
   - short hardware video
   - deviations/fallback notes

### Copy/Paste Message for Monday Chat Resume

```text
Resume Module 3 implementation from this checkpoint.

Context:
- Branch: feature/mobility_scenarios/V2I_and_emergencypriority
- Date checkpoint: 2026-06-03
- Local flows are DONE and validated:
  - V2I barrier flow
  - Emergency traffic-light flow
  - Unified coordinator flow
- Preflight command (already validated):
  - bash src/mobility_scenarios_src/run_preflight.sh
- Evidence files:
  - docs/guides/mobility_scenarios/shared/evidence/local_demo_2026-06-03.log
  - docs/guides/mobility_scenarios/shared/evidence/rules_validation_2026-06-03.log

Independent use-case rules:
- Barrier use case:
  - open -> advance
  - closed/unknown/timeout -> stop
- Traffic-light use case:
  - green/emergency_green -> advance
  - yellow -> slow_down
  - red/unknown/timeout -> stop

What to do now (hardware day):
1) Implement real Kitronik barrier control in src/mobility_scenarios_src/v2i/kitronik_barrier.py
2) Set barrier.backend = "hardware" in src/mobility_scenarios_src/v2i/config.json
3) Implement real traffic-light hardware path in src/mobility_scenarios_src/emergency_priority/trafficlight_simulator.py
4) Validate:
   - barrier closed=open/stop logic
   - traffic light green/yellow/red logic
   - emergency override in unified flow
5) Capture hardware logs + video evidence and update docs/guides/mobility_scenarios/shared/demo_script_module3.md

Safety fallback requirements:
- On timeout/error:
  - barrier -> closed
  - traffic light -> red
  - vehicle command -> stop
```
