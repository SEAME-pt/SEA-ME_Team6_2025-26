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
- [micro:bit Editor Choice (Python vs JavaScript)](#microbit-editor-choice-python-vs-javascript)
- [micro:bit Hardware Reference (V2.2X)](#microbit-hardware-reference-v22x)
- [Wireless Architecture (Phase 2/3 Remote Deployment)](#wireless-architecture-phase-23-remote-deployment)
- [Presentation Architecture Summary](#presentation-architecture-summary)
- [Hardware Hand-Off](#hardware-hand-off)
- [Session Consolidation Notes (June 9, 2026)](#session-consolidation-notes-june-9-2026)
- [Wireless Decision (BLE vs micro:bit radio)](#wireless-decision-ble-vs-microbit-radio)
- [BLE vs micro:bit radio in this scenario](#ble-vs-microbit-radio-in-this-scenario)
- [Path B Implementation Checklist (BLE Direct)](#path-b-implementation-checklist-ble-direct)
- [Path B Implementation (Detailed Status)](#path-b-implementation-ble-direct--in-progress)

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

## micro:bit Editor Choice (Python vs JavaScript)

This project currently uses two editor contexts for different goals:

- Python editor: https://python.microbit.org/v/3
- MakeCode editor: https://makecode.microbit.org/#editor

### Code format clarification

The snippet used for Path B is **JavaScript in MakeCode** (practically simplified TypeScript), not Python.

- The marker `blocks` means "code equivalent to blocks" in the MakeCode editor.
- It should be pasted in the MakeCode code editor, not in the Python editor page.

### Why we switched from python.microbit.org to makecode.microbit.org

- Before: MicroPython flow for simpler USB/serial scenarios.
- Now: Path B BLE (`UART over Bluetooth`) to communicate wirelessly with AGL.
- For this BLE UART prototype, MakeCode offers more direct integration:
  - `bluetooth.startUartService()`
  - `bluetooth.uartWriteText(...)`
  - quick block/JS workflow for rapid test iteration.

Practical summary:

- `python.microbit.org` is excellent for MicroPython scripts, but less direct for this BLE UART prototype path.
- `makecode.microbit.org` is better to get BLE UART running quickly on micro:bit V2.

## micro:bit Hardware Reference (V2.2X)

This section consolidates hardware information used in this project for BLE Path B decisions.

Primary sources:

- [micro:bit main site](https://microbit.org/)
- [micro:bit developer community](https://tech.microbit.org/)
- [micro:bit hardware overview](https://tech.microbit.org/hardware/)

### Hardware revisions context

- V2.2X (latest family used as reference)
- V2.00
- V1.5 / V1.3X

### Overview map

- About the BBC micro:bit
- Hardware block diagram
- Hardware description:
  - nRF52 application processor
  - Bluetooth wireless communication
  - low-level radio communications
  - buttons
  - display
  - motion sensor
  - temperature sensing
  - speaker
  - microphone
  - GPIO pins
  - power supply
  - interface and USB communications
  - debugging and mechanical information

### About the BBC micro:bit

The BBC micro:bit is a programmable single-board computer where user applications run on the nRF52 application processor. An interface processor handles USB communication and drag-and-drop flashing. Two core references for hardware internals are:

- [Schematics](https://tech.microbit.org/hardware/schematic)
- [Reference design](https://tech.microbit.org/hardware/reference-design)

Hardware visuals:

- Board overview image: `https://tech.microbit.org/docs/hardware/assets/microbit-overview-2-2.png`
- Block diagram image: `https://tech.microbit.org/docs/hardware/assets/v2-2-block.svg`

### nRF52 application processor

The user application, runtime, and Bluetooth stack run from on-chip flash on the nRF52. GPIO is provided by this processor.

| Field | Value |
|---|---|
| Model | Nordic nRF52833 |
| Core | Arm Cortex-M4 32-bit with FPU |
| Flash | 512 KB |
| RAM | 128 KB |
| CPU clock | 64 MHz |
| Debug | SWD |

### Bluetooth wireless communication

The on-board antenna supports BLE via Nordic S113 SoftDevice.

| Field | Value |
|---|---|
| Stack | Bluetooth 5.1 (BLE) |
| Band | 2.4 GHz ISM |
| Channels | 40 BLE channels (3 advertising: 37, 38, 39) |
| Sensitivity | -93 dBm (BLE mode) |
| TX power | -40 dBm to +4 dBm |
| Roles | GAP Peripheral and GAP Central |
| Congestion avoidance | Adaptive Frequency Hopping |
| Profile | BBC micro:bit profile |

### Low-level radio communications (micro:bit radio)

The same 2.4 GHz transceiver also supports non-BLE micro:bit radio protocol for simple broadcast packets.

| Field | Value |
|---|---|
| Protocol | micro:bit Radio |
| Frequency band | 2.4 GHz |
| Channel rate | 1 Mbps or 2 Mbps |
| Encryption | None |
| Channels | 80 |
| Group codes | 255 |
| TX power | 0 (-30 dBm) to 7 (+4 dBm) |
| Payload | 32 bytes (standard), up to 255 if reconfigured |

### Buttons

- Front A/B: user-programmable, software debounced.
- Rear button: system reset path (interface processor + nRF52).
- A/B use inverted logic with pull-up resistors.

### Display

- 5x5 red LED matrix.
- Software-driven refresh to avoid visible flicker.
- Also used for ambient light estimation via timing-based sensing.

### Motion and temperature

- Motion sensing: accelerometer + magnetometer (runtime supports multiple sensor variants).
- Gestures: hardware + runtime software algorithms.
- Temperature: exposed from nRF52 on-chip temperature sensor as ambient estimate.

### Relevance to this project

- BLE UART capability on V2.x hardware supports Path B (`micro:bit -> BLE -> AGL`).
- micro:bit radio capability exists, but is not natively consumable by AGL without gateway logic.
- Hardware capacity (nRF52833, 128 KB RAM, BLE 5.1) is adequate for the traffic light state broadcaster firmware used here.

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
- 2026-06-09: Template A firmware flashed on micro:bit using `microbit-mobility_traffic_lights.hex` (USB on Lenovo for power + flashing, BLE reserved for data path).
- 2026-06-09: AGL BLE scan executed; peripheral name `Trafficlight` and Nordic UART service UUID were not detected yet (`FOUND_TRAFFICLIGHT=False`, `FOUND_NUS=False`).
“hex flash feito”



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

## Path B Implementation (BLE Direct) — In Progress

### Important Notes on File Deployment & Architecture

**Question 2: Why is ble_trafficlight_receiver.py on PC + GitHub but not on AGL?**

Answer: **It's a deployment/tooling distinction.**
- **PC/GitHub:** These are the source files for version control and documentation
- **AGL:** These are runtime deployment locations
- **Solution:** The file must be COPIED from PC to AGL before testing (not cloned from GitHub directly)

**Recommended workflow:**
```bash
# On your PC (LENOVO)
cd ~/Documents/SEA-ME_Team6_2025-26
scp src/mobility_scenarios_src/emergency_priority/ble_trafficlight_receiver.py \
  root@10.21.220.191:/data/ADAS-Manager-tuning-trafficlight/
scp src/mobility_scenarios_src/emergency_priority/config_ble.json \
  root@10.21.220.191:/data/ADAS-Manager-tuning-trafficlight/
```

This deployment has already been completed and validated on the car AGL.

Once on AGL, the receiver runs from `/data/ADAS-Manager-tuning-trafficlight/` and listens for the BLE peripheral.

---

**Question 3: Why Two MakeCode Templates (A vs B)?**

This is a **staged validation approach** to reduce risk:

| Aspect | Template A | Template B |
|--------|-----------|-----------|
| **Goal** | Minimal BLE connection proof | Production-ready state machine |
| **Setup time** | 5 minutes | 10 minutes |
| **Complexity** | Single state (RED only) | Full state machine + timeouts |
| **Use case** | "Does BLE even work?" | "Is this ready for vehicle?" |
| **Why separate?** | Isolate connection issues from logic issues | |
| **Practical path** | Flash A today → test connection → then upgrade to B | |

**Rationale:**
- If Template A fails, you know the problem is BLE layer (Bluetooth adapter, permissions, range, etc.)
- If Template A works but B doesn't, you know the problem is MicroPython syntax/logic
- If you try to debug both together, you can't isolate root causes
- For Friday demo: if BLE is working, use B; if BLE fails, fallback to USB-direct (Path A)

---

### Implementation Files

**On AGL (Raspberry Pi 5):**
- `src/mobility_scenarios_src/emergency_priority/ble_trafficlight_receiver.py` — Python central receiver
  - Runtime location selected for the car: `/data/ADAS-Manager-tuning-trafficlight/`
  - Requires: `pip install bleak`
  - Logic: scan → connect → subscribe → inject `/tmp/adas_objects.sock`
  - Safety: timeout auto-RED, reconnect on disconnect

**On Micro:bit (V2.21):**
- MakeCode BLE peripheral (templates embedded below)
- Flash via: https://makecode.microbit.org (USB-C cable)
- Service: Nordic UART (6e400001-b5a3-f393-e0a9-e50e24dcca9e)

**Configuration:**
- `src/mobility_scenarios_src/emergency_priority/config_ble.json`
  - BLE service UUIDs, ADAS socket path, timeouts

**Tests:**
- `src/mobility_scenarios_src/emergency_priority/test_ble_receiver.py` (Python logic validation, no hardware)

### MakeCode Template A: Quick Prototype (5-minute BLE validation)

**Steps to flash:**
1. Go to https://makecode.microbit.org
2. Create new project
3. Add BLE extension
4. Copy this code:

```blocks
bluetooth.setAdvertisedName("Trafficlight")
bluetooth.startUartService()

let light_state = "R"

basic.forever(function () {
    bluetooth.uartWriteText(light_state)
    basic.pause(1000)
})

input.onButtonPressed(Button.A, function () {
    if (light_state == "R") {
        light_state = "Y"
    } else if (light_state == "Y") {
        light_state = "G"
    } else {
        light_state = "R"
    }
})
```

**What it does:**
- Advertises as "Trafficlight"
- Sends state (R/Y/G) every 1 second
- Button A cycles: R → Y → G → R
- Default startup: RED

### MakeCode Template B: Production State Machine (full safety rules)

**Steps to flash:**
1. Go to https://makecode.microbit.org
2. Create new project
3. Add BLE extension
4. Copy this code:

```blocks
bluetooth.setAdvertisedName("Trafficlight")
bluetooth.startUartService()

let current_state = "R"
let yellow_start_time = 0
const YELLOW_TIMEOUT_MS = 2000

input.onStart(function () {
    current_state = "R"
    led.plotBarGraph(1000, 1023)  // Red LEDs
})

bluetooth.onBluetoothConnected(function () {
    basic.showIcon(IconNames.Happy)
})

bluetooth.onBluetoothDisconnected(function () {
    current_state = "R"
    led.clear()
})

basic.forever(function () {
    // Send state every 500ms
    bluetooth.uartWriteText(current_state)
    basic.pause(500)

    // Yellow timeout escalation
    if (current_state == "Y") {
        if (input.runningTime() - yellow_start_time > YELLOW_TIMEOUT_MS) {
            current_state = "R"
            led.plotBarGraph(1000, 1023)
        }
    }
})

// Button A: RED
input.onButtonPressed(Button.A, function () {
    current_state = "R"
    led.plotBarGraph(1000, 1023)
})

// Button B: YELLOW
input.onButtonPressed(Button.B, function () {
    current_state = "Y"
    yellow_start_time = input.runningTime()
    led.plotBarGraph(700, 1023)
})

// Buttons A+B: GREEN
input.onButtonPressed(Button.AB, function () {
    current_state = "G"
    led.plotBarGraph(100, 1023)
})
```

**What it does:**
- Full state machine (RED, YELLOW, GREEN)
- Yellow auto-escalates to RED after 2 seconds
- Sends state every 500ms
- Button A: RED, Button B: YELLOW, Button A+B: GREEN
- Disconnection forces RED (safe default)
- LED visual feedback

### Testing Procedure (Tuesday/Wednesday)

### Powering the micro:bit during BLE testing

If the goal is **power only** and all data must stay **wireless**, the best practical setup is:

- **Keep the micro:bit connected to the Lenovo by USB** while flashing/testing firmware.
- Let the **AGL communicate only over BLE**.
- Do **not** use the USB serial path during these Path B tests.

Why this is the better choice right now:

- The Lenovo is already the machine you use to flash MakeCode firmware.
- It keeps the power supply stable while you iterate quickly.
- It avoids introducing an unnecessary cable change while debugging BLE.
- USB being plugged into the Lenovo does **not** prevent BLE operation; it only powers the board unless you intentionally use serial.

Connecting the micro:bit to the AGL only for power is also valid, but it gives no testing advantage at this stage. It only makes sense later if you want the final demo wiring to look cleaner.

**Step 1: Setup AGL (now)**
```bash
cd /data/ADAS-Manager-tuning-trafficlight

# Install dependency
pip install bleak

# Verify Bluetooth adapter
bluetoothctl show
```

**Step 2: Flash Template A to micro:bit (now)**
- Open https://makecode.microbit.org
- Copy Template A code above
- Download `.hex` file
- Flash via USB-C to micro:bit V2

Flash completion checklist (executed):
- micro:bit connected to Lenovo via USB
- `.hex` copied to `MICROBIT` drive
- rear LED stopped blinking (flash complete)
- micro:bit kept powered via USB (data path remains BLE)

**Step 3: Run BLE receiver on AGL (now)**
```bash
cd /data/ADAS-Manager-tuning-trafficlight

python3 ble_trafficlight_receiver.py \
  --config config_ble.json \
  --device-name "Trafficlight" \
  --verbose
```

Expected output:
```
Scanning for device: Trafficlight...
Found device: Trafficlight (XX:XX:XX:XX:XX:XX)
Connecting to XX:XX:XX:XX:XX:XX...
Connected to BLE device
Subscribed to 6e400003-b5a3-f393-e0a9-e50e24dcca9e
BLE notification: raw=52, state=RED
Injected ADAS object: SIGN_TL_RED
```

**Step 4: Verify socket injection (now)**
```bash
# Terminal 2 on AGL
socat - UNIX-CONNECT:/tmp/adas_objects.sock

# Expected output (every 1-2 seconds):
# {"class": "SIGN_TL_RED", "confidence": 1.0, "state": "RED", ...}
```

**Step 5: Test state changes (now)**
- On micro:bit, press button A repeatedly to cycle R → Y → G → R
- On AGL terminal 2, watch socket receive each state change

### Status & Next Steps

**✅ Completed:**
- Python BLE receiver written (ble_trafficlight_receiver.py)
- MakeCode templates designed (Template A minimal + Template B production)
- Configuration template ready (config_ble.json)
- Unit tests created (test_ble_receiver.py)
- Documentation consolidated into planning_V2I.md
- File deployment plan documented
- BLE receiver deployed to AGL runtime folder: `/data/ADAS-Manager-tuning-trafficlight/`
- AGL runtime validated:
  - hostname confirmed: `seame-agl`
  - Bluetooth adapter available and powered
  - ADAS socket exists at `/tmp/adas_objects.sock`
  - Python dependency `bleak` installed on AGL
  - remote syntax validation passed (`python3 -m py_compile ble_trafficlight_receiver.py`)

**⏳ In Progress (Immediate Actions):**
1. **Verify firmware advertising settings** (name + UART service enabled)
2. **Re-run BLE scan** (detect by name and/or service UUID)
3. **Test BLE connection** (scan → connect → receive state)
4. **Validate socket injection** (monitor `/tmp/adas_objects.sock`)

**❌ Pending (After BLE Validated):**
- Upgrade to Template B (full state machine)
- ADAS Manager integration and throttle override
- Vehicle motion validation (stop/slow/go)
- Friday presentation readiness (BLE OR fallback to USB-direct)

---

## Next Actions — Detailed Implementation Roadmap

### Action 1: Deploy BLE Receiver to AGL (Today — 15 min)

This action is already completed.

**Completed deployment details:**
```bash
# Files deployed to AGL runtime folder
/data/ADAS-Manager-tuning-trafficlight/ble_trafficlight_receiver.py
/data/ADAS-Manager-tuning-trafficlight/config_ble.json

# Dependency installed on AGL
python3 -m pip install bleak
```

**Verified on AGL:**
```bash
hostname                      # seame-agl
ls -la /data/ADAS-Manager-tuning-trafficlight/
ls -l /tmp/adas_objects.sock
python3 -m py_compile /data/ADAS-Manager-tuning-trafficlight/ble_trafficlight_receiver.py
```

---

### Action 2: Flash Template A Firmware to Micro:bit (Today — 10 min)

**On your PC:**
1. Keep the micro:bit V2 connected to the Lenovo via USB-C for power + flashing
2. Open https://makecode.microbit.org in browser
3. Create new project
4. In MakeCode, use the official Bluetooth API:
  - `Advanced` → `Bluetooth` (preferred), or
  - `Extensions` → search `Bluetooth` and pick the official micro:bit Bluetooth package
  - Do **not** use `bsiever/microbit-pxt-blehid` (HID extension for keyboard/mouse/gamepad)
5. Delete default code blocks
6. Copy Template A code from [MakeCode Template A section above](#makecode-template-a-quick-prototype-5-minute-ble-validation)
7. Click **Download** → save `.hex` file
8. Drag `.hex` onto MICROBIT drive (appears as USB device)
9. Wait for board to blink/reset

**Verify on micro:bit:**
- All LEDs should flash initially
- Pressing button A should cause LED feedback

**Flash completion checklist (copy/paste):**
1. Connect micro:bit to Lenovo via USB (power + flashing only).
2. Copy `.hex` to `MICROBIT` drive (drag and drop).
3. Wait until rear LED stops blinking (flash completed).
4. Keep micro:bit connected by USB for power; data path remains BLE.

---

### Action 3: Test BLE Connection (Today — 10 min)

**Terminal 1 on AGL (start receiver):**
```bash
cd /data/ADAS-Manager-tuning-trafficlight

python3 ble_trafficlight_receiver.py \
  --config config_ble.json \
  --device-name "Trafficlight" \
  --verbose
```

**Expected output (within 10-15 seconds):**
```
[INFO] BLE Receiver initialized (device: Trafficlight, timeout: 5.0s)
[INFO] ADAS socket: /tmp/adas_objects.sock
[INFO] Scanning for device: Trafficlight...
[INFO] Found device: Trafficlight (XX:XX:XX:XX:XX:XX)
[INFO] Connecting to XX:XX:XX:XX:XX:XX...
[INFO] Connected to BLE device
[INFO] Subscribed to 6e400003-b5a3-f393-e0a9-e50e24dcca9e
[DEBUG] BLE notification: raw=52, state=RED
[DEBUG] Injected ADAS object: SIGN_TL_RED
```

**If no connection after 30s:**
- Check micro:bit is powered on (plug USB-C in again)
- Verify "Trafficlight" appears in `bluetoothctl scan on` on AGL
- Check Bluetooth adapter: `bluetoothctl show` (should show adapter name/MAC)
- Try restarting micro:bit (press RESET button on back)

---

### Action 4: Verify Socket Injection (Today — 5 min)

**Terminal 2 on AGL (same machine as Action 3):**
```bash
# Watch traffic-light objects arrive on socket
socat - UNIX-CONNECT:/tmp/adas_objects.sock
```

**Expected output (every 1 second from Terminal 1):**
```json
{"class": "SIGN_TL_RED", "confidence": 1.0, "state": "RED", "timestamp": 1717939234.567}
```

**If nothing arrives:**
- Check receiver is running in Terminal 1 (should show "Subscribed" message)
- Verify receiver is not logging errors
- Check socket exists: `ls -la /tmp/adas_objects.sock`

---

### Action 5: Test State Changes (Today — 10 min)

**On micro:bit (physically):**
- Press button A repeatedly to cycle: R → Y → G → R

**On Terminal 2 (socat output):**
- Watch state change in socket output every time you press button A
- State should go: RED → YELLOW → GREEN → RED

**Expected sequence in socat:**
```json
{"class": "SIGN_TL_RED", ...}     # Initial state
{"class": "SIGN_TL_YELLOW", ...}  # After button A once
{"class": "SIGN_TL_GREEN", ...}   # After button A twice
{"class": "SIGN_TL_RED", ...}     # After button A thrice (cycling back)
```

---

### Action 6: Validate Timeout Safety (Today — 10 min)

**On Terminal 2 (socat):**
1. Watch state updates (should see RED every 1 second)
2. Turn off micro:bit (remove USB power) while receiver is running
3. Wait 5+ seconds
4. Check socat output

**Expected behavior:**
- After ~5 seconds with no signal, receiver should force state to RED
- Socat should show: `{"class": "SIGN_TL_RED", "state": "RED", ...}`
- When you reconnect micro:bit, receiver should re-establish connection and resume normal updates

**This validates:** receiver doesn't hang on lost connection; it safely defaults to RED

---

### Action 7: Ready for Template B Upgrade (Wednesday)

Once Actions 1-6 all pass:
1. Update MakeCode project: delete Template A code
2. Copy Template B code (from [MakeCode Template B section](#makecode-template-b-production-state-machine-full-safety-rules))
3. Add yellow timeout test: hold button B for 2+ seconds and watch it auto-escalate to RED
4. Test disconnection safety (turn off micro:bit → should force RED on AGL)

---

### Timeline Summary

| Stage | Target Date | Goal |
|-------|-------------|------|
| **Actions 1-3** | Today (Tue) | BLE connection working |
| **Actions 4-6** | Today (Tue) | Socket injection working + safety validated |
| **Action 7** | Tomorrow (Wed) | Template B upgraded + full state machine tested |
| **ADAS integration** | Wed/Thu | Vehicle throttle override + motion validation |
| **Friday demo** | Friday | Both BLE + USB-direct paths ready; present preferred (BLE) |

---

## Documentation Update Log

- **Monday, June 8:** USB-direct baseline validated (serial connection working)
- **Tuesday, June 9 (now):** 
  - Path B BLE architecture designed
  - MakeCode templates created (A + B)
  - Python receiver implemented
  - Consolidated into `planning_V2I.md`
  - Deployment plan documented
  - Staged testing roadmap created
  - BLE receiver deployed to AGL at `/data/ADAS-Manager-tuning-trafficlight/`
  - AGL Bluetooth adapter verified and `bleak` installed
  - Remote receiver syntax validated successfully on AGL
  - Runtime path decision documented: `/data/ADAS-Manager-tuning-trafficlight/` chosen over `/home/seame/...`
  - Hostname confirmed as `seame-agl`
  - `/tmp/adas_objects.sock` confirmed on the car AGL
  - Powering recommendation documented: keep micro:bit on Lenovo USB for power/flashing while using BLE only for data
  - Template A flash execution checklist documented (USB power on Lenovo, `.hex` copied, board running)
  - AGL scan attempt logged: peripheral not yet detected by advertised name or NUS UUID

---

---

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
