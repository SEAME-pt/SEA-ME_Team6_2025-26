# Module 3: V2I Integration & Emergency Priority — Master Planning Guide

**Single source of truth** for Vehicle-to-Infrastructure communication, emergency traffic light control, and unified coordination.

**Status:** Phase 3 Implementation Complete - Hardware validation in progress (Tuesday)  
**Owner:** Joao  
**Branch:** `feature/mobility_scenarios/V2I_and_emergencypriority`  
**Last Updated:** Tuesday, June 9, 2026

## Quick Navigation

- [Current Status](#current-status)
- [Implementation Phase Overview](#implementation-phase-overview)
- [MicroPython Context](#micropython-context)
- [Wireless Architecture (Phase 2/3 Remote Deployment)](#wireless-architecture-phase-23-remote-deployment)
- [Presentation Architecture Summary](#presentation-architecture-summary)
- [Hardware Hand-Off](#hardware-hand-off)
- [Session Consolidation Notes (June 9, 2026)](#session-consolidation-notes-june-9-2026)
- [Wireless Decision (BLE vs micro:bit radio)](#wireless-decision-ble-vs-microbit-radio)
- [BLE vs micro:bit radio in this scenario](#ble-vs-microbit-radio-in-this-scenario)
- [Path B Implementation Checklist (BLE Direct)](#path-b-implementation-checklist-ble-direct)

## MicroPython Context

### What is MicroPython?

MicroPython was **not created by this team**.

- MicroPython is a lightweight implementation of Python 3 for microcontrollers.
- The original creator is **Damien P. George**.
- It is an open-source project maintained by the wider community.

### How it is used in this project

- On the micro:bit, the MicroPython firmware runs inside the board and executes `.py` scripts.
- In this project, `src/mobility_scenarios_src/emergency_priority/microbit_trafficlight_firmware.py` is **project application code** running on top of the community MicroPython runtime.

### Simple summary

- MicroPython = the Python runtime/system running on the micro:bit.
- Our script = the application code running on that runtime.

## Current Status

- [x] Phase 1: V2I barriers MVP complete with tests
- [x] Phase 2: Emergency traffic lights MVP complete with tests
- [x] Phase 3: Unified coordinator complete with integration tests
- [x] USB-direct bridge architecture validated and deployed
- [x] AGL folder (`/data/ADAS-Manager-tuning-trafficlight`) ready
- [x] ADAS socket infrastructure verified
- [x] All documentation consolidated and translated to English
- [x] Evidence logs collected: `docs/guides/mobility_scenarios/shared/evidence/`

## Implementation Phase Overview

### Phase 1: V2I Barriers MVP ✅ COMPLETE

**Goal:** Vehicle requests barrier opening and receives status feedback.

**Key Files:**
- `src/mobility_scenarios_src/v2i/barrier_simulator.py`
- `src/mobility_scenarios_src/v2i/barrier_rules.py`
- `src/mobility_scenarios_src/v2i/barrier_backend.py`
- `src/mobility_scenarios_src/v2i/kitronik_barrier.py` (future hardware)

**Acceptance Criteria:**
- ✅ Barrier closes on timeout (safe fail default)
- ✅ Vehicle receives state confirmation
- ✅ Status visible in KUKSA/VSS
- ✅ Unit & integration tests passing

---

### Phase 2: Emergency Priority (Traffic Lights) ✅ COMPLETE

**Goal:** Emergency vehicle forces green traffic light with safety override.

**Key Files:**
- `src/mobility_scenarios_src/emergency_priority/trafficlight_simulator.py`
- `src/mobility_scenarios_src/emergency_priority/traffic_light_rules.py`
- `src/mobility_scenarios_src/emergency_priority/emergency_client.py`
- **`src/mobility_scenarios_src/emergency_priority/trafficlight_vehicle_bridge.py`** ← Main Bridge
- `src/mobility_scenarios_src/emergency_priority/microbit_trafficlight_firmware.py` (MicroPython code)

**Bridge Modes (3 Options):**

```
LOCAL MODE (Validated Path) ✅
  Micro:bit → USB /dev/ttyACMx → Single machine bridge → ADAS socket
  Deployment: USB cable from micro:bit to AGL

TRANSMITTER MODE (Future - PC Side)
  Micro:bit → USB /dev/ttyACM0 → PC bridge → UDP:5555 → Network

RECEIVER MODE (Future - AGL Side)
  Network UDP:5555 → AGL bridge → /tmp/adas_objects.sock → ADAS Manager
```

**Safety Features:**
- Yellow timeout escalation (2.0s default) → auto-RED
- Heartbeat monitoring (5.0s timeout) → safe RED
- Non-blocking socket writes
- Fallback on any error → RED (safe stop)

**Acceptance Criteria:**
- ✅ Emergency trigger → green
- ✅ Normal timeout → RED
- ✅ Yellow escalates → RED after 2s
- ✅ State propagation to ADAS Manager confirmed
- ⏳ Real vehicle throttle change (with physical hardware)

---

### Phase 3: Unified V2I + Emergency Priority ✅ COMPLETE

**Goal:** Single coordinator orchestrates barriers, lights, emergency override.

**Key Files:**
- `src/mobility_scenarios_src/emergency_priority/coordinator.py`
- `src/mobility_scenarios_src/emergency_priority/unified_demo.py`

**Priority Logic:**
1. Emergency Mode: Forces barrier OPEN + light GREEN (override)
2. Normal Mode: Barriers by request, lights by state
3. Fallback: Barrier CLOSED + light RED on timeout

**Validation:**
```bash
python3 src/mobility_scenarios_src/emergency_priority/unified_demo.py --config config.json
# Expected: Step 1 (normal) → Step 2 (emergency) → Step 3 (return) → PASS
```

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

## Presentation Architecture Summary

This diagram is intended for Friday presentation use. It explains the implementation at a system level and highlights both the current validated path and the planned wireless path.

```text
                            MODULE 3 IMPLEMENTATION OVERVIEW

  INFRASTRUCTURE SIDE                                      VEHICLE SIDE

  ┌──────────────────────────┐                             ┌──────────────────────────┐
  │ micro:bit V2.21          │                             │ AGL / Raspberry Pi 5     │
  │                          │                             │                          │
  │ Current validated path:  │                             │ receiver / bridge logic  │
  │ - MicroPython firmware   │                             │ - parses light state     │
  │ - traffic light control  │                             │ - safety timeout logic   │
  │ - red / yellow / green   │                             │ - fallback to RED        │
  └─────────────┬────────────┘                             └─────────────┬────────────┘
                │                                                          │
                │ Path A: USB serial                                       │
                ├──────────────────────────────────────────────────────────► │
                │                                                          │
                │ Path B: BLE direct (planned)                             │
                ├──────────────────────────────────────────────────────────► │
                │                                                          │
                ▼                                                          ▼
        ┌───────────────────┐                                     ┌──────────────────────┐
        │ traffic light     │                                     │ /tmp/adas_objects.sock│
        │ event generated   │                                     │ object injection path │
        └───────────────────┘                                     └──────────┬───────────┘
                                                                                │
                                                                                ▼
                                                                     ┌──────────────────────┐
                                                                     │ adas_manager (C++)   │
                                                                     │ - receives sign class│
                                                                     │ - sets throttle      │
                                                                     │ - publishes to CAN   │
                                                                     └──────────┬───────────┘
                                                                                │
                                                                                ▼
                                                                     ┌──────────────────────┐
                                                                     │ STM32 / ThreadX      │
                                                                     │ - motor actuation    │
                                                                     │ - steering/throttle  │
                                                                     └──────────┬───────────┘
                                                                                │
                                                                                ▼
                                                                     ┌──────────────────────┐
                                                                     │ Vehicle motion       │
                                                                     │ stop / slow / go     │
                                                                     └──────────────────────┘

  Optional parallel path:
  AGL bridge -> KUKSA/VSS -> Qt Cluster display
```

## Path B Implementation Checklist (BLE Direct)

This section defines the direct BLE path without an intermediate network transmitter process.

### 1. Firmware on micro:bit

Choose one of the following:

- **MakeCode BLE path**: preferred for fastest BLE prototype.
- **C++ custom firmware path**: preferred only if tighter BLE control is required.

Required firmware behavior:

- Advertise as a **BLE peripheral**.
- Expose a characteristic carrying traffic-light state.
- Send one of: `RED`, `YELLOW`, `GREEN`.
- Default startup state must be `RED`.

### 2. BLE service and characteristic design

Recommended options:

- **Option 1: Nordic UART Service (NUS)**
  - Good for fast prototyping.
  - AGL subscribes to notifications.
- **Option 2: Custom GATT service**
  - Better long-term clarity for product architecture.
  - Characteristic payload could be a single byte or short text state.

Recommended payloads:

- `R` = red
- `Y` = yellow
- `G` = green

Optional fields:

- sequence number
- timestamp
- heartbeat flag

### 3. Receiver on AGL

The AGL/Pi5 acts as **BLE central**.

Receiver responsibilities:

- scan for the target peripheral
- connect automatically
- subscribe to notifications
- parse incoming state
- translate state to ADAS object class
- write to `/tmp/adas_objects.sock`
- optionally publish to KUKSA/VSS

Recommended runtime path:

- `BLE central receiver -> /tmp/adas_objects.sock -> adas_manager -> CAN -> vehicle`

### 4. Timeout, reconnect, and safety RED behavior

Required safety rules:

- If the BLE receiver loses connection, force `RED`.
- If no BLE message arrives within configured timeout, force `RED`.
- On startup, before first valid message, assume `RED`.
- Reconnect automatically after disconnect.
- Log all disconnect, reconnect, and timeout events.

Suggested values:

- heartbeat interval: `1.0s`
- timeout to safe RED: `5.0s`
- yellow escalation timeout: `2.0s`

### 5. End-to-end test commands

Example staged validation plan:

#### On micro:bit

- flash BLE-capable firmware (MakeCode or C++)
- confirm peripheral is advertising

#### On AGL

```bash
# Verify Bluetooth adapter is up
bluetoothctl show

# Run BLE receiver service
python3 ble_trafficlight_receiver.py --config config.json
```

Expected behavior:

- connect to micro:bit peripheral
- receive state notifications
- inject traffic-light object into `/tmp/adas_objects.sock`

#### ADAS validation

```bash
# In parallel on AGL
/data/ADAS-Manager-tuning-trafficlight/adas_manager
```

#### Functional test sequence

1. Send `RED` -> vehicle must stop.
2. Send `YELLOW` -> vehicle must slow, then stop after timeout if still yellow.
3. Send `GREEN` -> vehicle may continue.
4. Turn off peripheral or break BLE link -> receiver must force `RED`.

### 6. Delivery recommendation

- **For Friday presentation**: present BLE direct as the preferred wireless target architecture.
- **For deterministic demo execution**: keep USB-direct as the currently validated path.
- **For next implementation step**: prototype BLE first with MakeCode before deciding on custom C++ firmware.

## Hardware Hand-Off

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

### What Is Still Pending for Hardware Day

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

### Copy/Paste Message for Hardware Chat Resume

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

## Session Consolidation Notes (June 9, 2026)

This section captures the key operational notes that were previously recorded in a separate session summary file.

### Q1: Is a real test possible without connecting the traffic light to AGL?

Short answer: no for full end-to-end validation.

- What is validated without hardware:
  - Bridge logic and rules behavior.
  - ADAS socket path and config loading.
  - Local simulation flows and unit tests.
- What requires physical micro:bit to AGL USB:
  - Real serial communication on `/dev/ttyACM1`.
  - Live throttle override propagation to ADAS/CAN.
  - Timeout and heartbeat behavior under real signal loss.

### Q2: Documentation language policy

All planning and integration documentation for this module must be in English.

### Consolidation result

- Single master document retained: `docs/guides/mobility_scenarios/planning_V2I.md`.
- Legacy duplicate docs were removed during consolidation.
- Evidence logs remain in: `docs/guides/mobility_scenarios/shared/evidence/`.

### Current readiness snapshot

- Branch has implementation and documentation pushed.
- USB-direct deployment path is the approved low-risk path for immediate hardware tests.
- BLE remains a future enhancement because micro:bit MicroPython BLE UART is not available for this flow.

### Wireless Decision (BLE vs micro:bit radio)

- **BLE with MicroPython on micro:bit V2.21:** not suitable for this project path because custom BLE UART service is not available in standard MicroPython runtime.
- **micro:bit `radio` module:** feasible between micro:bits, but not directly consumable by AGL without an additional gateway node; this adds architecture complexity now.
- **Recommended immediate wireless test:** keep current `transmitter`/`receiver` bridge design (serial from micro:bit to host, then UDP to AGL) to validate wireless behavior without firmware rewrite.

Decision: proceed with USB-direct for deterministic end-to-end validation first; run bridge-based wireless test next if needed.

### BLE vs micro:bit radio in this scenario

#### BLE

Advantages:

- Compatible directly with AGL/Pi5 (BlueZ).
- Standardized architecture (GATT), more industrial-grade.
- Supports security/pairing/encryption.
- No extra hardware is required beyond micro:bit + Pi5.

Disadvantages:

- More complex implementation.
- For this practical use case on micro:bit, you typically need to move away from standard MicroPython runtime (MakeCode or C++ firmware path).
- Initial debugging can take time (pairing, reconnection, MTU, etc.).

#### micro:bit radio

Advantages:

- Very simple between micro:bit devices.
- Low latency and quick setup in educational environments.

Disadvantages:

- Not standard BLE protocol.
- Pi5/AGL does not natively consume micro:bit-radio frames.
- In practice, you need an additional gateway (another micro:bit or a dedicated firmware/bridge layer).

#### Which is better for this project?

For wireless communication in the path `micro:bit -> AGL -> vehicle`, BLE is the better long-term option.

micro:bit radio only makes sense if adding a gateway is acceptable.
