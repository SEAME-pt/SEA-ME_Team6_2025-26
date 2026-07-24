# ADAS Manager

## Overview

C++ process that receives perception data (lane, objects, V2I) via UNIX DGRAM sockets, drives the car in MANUAL or AUTONOMOUS mode (LKA + Obstacle Avoidance + ACC + object/V2I throttle limiting), sends unified control commands to the STM32 via CAN bus, and publishes telemetry to the KUKSA Databroker via an asynchronous bridge thread. A joystick client (`joystick_control.py`) drives manual mode, mode toggles, and an emergency-priority-vehicle toggle over its own socket.

```
[INFERENCE_DUAL.py]              [joystick_control.py]        [V2I / mobility scenarios]
    │ LaneFrame   → /tmp/adas_lane.sock                              │ V2IFrame → /tmp/adas_v2i.sock
    │ ObjectFrame → /tmp/adas_objects.sock                           │
    ▼                                  │ J/T/M/U/E → /tmp/adas_joystick.sock
[adas_manager — C++]                   ▼
    ├── lane_thread    — receives LaneFrame
    ├── object_thread  — receives ObjectFrame
    ├── v2i_thread     — receives V2IFrame
    ├── joystick_thread — receives drive + mode + emergency-toggle msgs
    ├── status_thread  — receives CtrlStatus_t (0x213) from STM32: speed + SRF08 gap
    └── main loop (50Hz tick, 20ms sleep)
            │
            ├── watchdogs (lane/obj/v2i/joy timeouts → invalidate stale data)
            ├── drive mode toggle (MANUAL ⇄ AUTONOMOUS, or forced by joystick)
            ├── emergency-priority toggle → EmergencySender → roadside_emergency_controller.py
            │
            ├── MANUAL   → manual_driving()      (joystick passthrough)
            └── AUTONOMOUS → adas_state_machine() (INIT → ACTIVE → DEGRADED → EMERGENCY_STOP)
                           → autonomous_driving()  (LKA + OA + ACC + curve slowdown +
                                                     object/V2I throttle limit)
            │
            ├── heartbeat 0x700 @ 10Hz
            ├── bridge.pub_lane() / pub_objects() / pub_mode()  ← non-blocking, ≤5Hz
            ▼
        CanSender → can1
            │ 0x202: CtrlCmd_t   (mode + steering + throttle + target_speed + headway + aeb_request)
            │ 0x700: Heartbeat_t
            │ 0x001: EmergencyStop_t (active + CRC-8)
            ▼
        [STM32 / ThreadX]
            │ 0x213: CtrlStatus_t → status_thread (current_speed_cms, gap_cm)
            ▼
        bridge_thread (KuksaBridge)
            │ queue<string> max 4 msgs
            │ fputs + fflush → pipe stdin
            ▼
        [kuksa_bridge.py — subprocess]
            │ gRPC v2 TLS + JWT
            ▼
        [KUKSA Databroker]
```

Single-instance lock (`/tmp/adas_manager.lock`, `flock`) prevents two managers fighting over the joystick socket / CAN bus.

---

## Drive Modes

`DriveMode::MANUAL` / `DriveMode::AUTONOMOUS` — toggled by joystick key `t`, or forced directly by `m` (force MANUAL) / `u` (force AUTONOMOUS). Forcing resets the ADAS state machine and LKA/OA controllers.

- **MANUAL** (`manual_driving`) — passes joystick steering/throttle straight to CAN as `CTRL_MODE_MANUAL`. No joystick frame within `joy_timeout_ms` → `CTRL_MODE_DISABLED`.
- **AUTONOMOUS** (`autonomous_driving`) — driven by the ADAS state machine below.

---

## ADAS State Machine (AUTONOMOUS only)

`AdasState`: `INIT → ACTIVE → DEGRADED → EMERGENCY_STOP`, defined in `adas_state_machine.hpp`.

![alt text](ADAS-State-Machine.png)

- **ACTIVE** — `lane_ok` (valid lane frame + `lane_status != 0`), or an OA maneuver is in progress (leaving the lane on purpose doesn't count as lane loss). LKA computes steering every new lane frame.
- **DEGRADED** — lane lost for `degraded_threshold_frames` (10) consecutive ticks. Car keeps driving with `lka.last_steering()` (soft reset — no snap to centre). If lane stays lost for `emergency_threshold_ms` (1500ms) while in DEGRADED → EMERGENCY_STOP.
- **EMERGENCY_STOP** — sends `aeb_request=true` every tick (STM32 has final say on braking) and one-shot `0x001 EmergencyStop_t`. Recovers to ACTIVE after `recovery_threshold_frames` (15) consecutive good lane frames. **Deadlock breaker:** if stuck in EMERGENCY_STOP for `emergency_timeout_ms` (10000ms) — e.g. STM32's own AEB stopped the car somewhere the camera can't see lane — force-drops to DEGRADED (not ACTIVE) so `aeb_request` stops firing and the STM32 is freed; re-arms EMERGENCY_STOP if lane is still lost after another `emergency_threshold_ms`.

---

## Obstacle Avoidance (OA)

`oa_controller.hpp` — `OAController`, enabled via `oa_enabled` in config. Runs only in ACTIVE/DEGRADED, before LKA (freezes LKA's integrator during the maneuver).

Fixed-geometry evade computed once on trigger, then executed open-loop by timer:

- **IDLE** → trigger if SRF08 gap (`< oa_critical_dist_m`) or a camera-seen `SIGN_OBSTACLE` is in-path (`d_cam_m < critical_dist_m` and lateral offset `< car_width/2 + margin`). Computes a single steering angle from bicycle-model geometry (wheelbase, target lateral offset) via `atan(wheelbase/R)`, normalised to servo range. If required angle exceeds `servo_max_deg` → `BLOCKED` (feeds `aeb_request`) instead of evading.
- **EVADE** (`evade_ms`) → steer at computed angle.
- **STRAIGHT** (`straight_ms`) → steering=0, still at evade throttle.
- **RETURN** (`return_ms`) → steer at `-evade_angle` to re-merge, then back to IDLE.

**Adaptive timing:** `evade_ms`/`straight_ms`/`return_ms` are linearly interpolated between a `_lo` and `_hi` calibration setpoint based on current `cfg.throttle` (faster car → shorter timings), recomputed via `adapt_timings()` each time OA is idle.

Mid-maneuver abort: if the camera reports a hard-stop condition (`throttle_limit == 0` — collision, STOP sign, red light) while evading, OA resets and the car stops instead of continuing the swerve.

---

## Adaptive Cruise Control (ACC)

Optional (`acc_enabled`, default off — LKA mode uses fixed `cfg.throttle` instead). When on, ACTIVE state sends `CTRL_MODE_ACC` with a `target_speed_cms` computed from `acc_target_kmh` scaled by curve/single-line/throttle-limit factors, plus a `headway` setting (`acc_headway`: 1=close/2=medium/3=far). STM32 owns the closed speed loop using SRF08 lead-gap feedback.

---

## Curve Slowdown

Applied in ACTIVE (both LKA and ACC), skipped during OA maneuvers. `factor = max(curve_min_factor, 1 - curve_gain * |steering|/100)`, EMA-smoothed (`curve_ema_alpha`). Combined (via `min`, not multiply, to avoid stacking) with `single_line_factor` when only one lane line is visible. In LKA mode the anti-stall floor `curve_throttle_min` guarantees the motor never gets a nonzero-but-too-low throttle that stalls it.

---

## LKA Controller

`lka_controller.hpp` — PID (`kp`/`ki`/`kd`) + EMA smoothing + soft deadband + centre-snap + rate limit, ported from `lka_steering_v1_2.py`.

Runs **once per new lane frame** (not per 50Hz tick) with `dt` = real inter-frame interval, clamped to `[0.05, 0.5]`s — running the D-term at 50Hz against a value that only updates at ~6-10fps was overestimating derivative ~8× and causing steering spikes. Between frames, `compute()` returns the last steering value; the 50Hz loop keeps re-sending it to CAN.

- EMA smoothing seeds from the raw reading on the first frame after a reset (converging from 0 was costing ~3 frames of lag on DEGRADED recovery).
- Deadband is a continuous ramp, not a hard step, to avoid a derivative spike at the deadband edge.
- `reset_soft()` clears PID/EMA state but **keeps `last_steering_`** — used on DEGRADED entry and OA-maneuver end so the wheels don't snap to centre mid-curve.

### Parameters (`lka_config.conf`)

| Parameter | Default | Description |
|---|---|---|
| `kp` | 4.0 | Proportional gain |
| `ki` | 0.0 | Integral gain |
| `kd` | 2.0 | Derivative gain |
| `ema_alpha` | 0.5 | EMA smoothing (0=frozen, 1=no smoothing) |
| `deadband` | 2.0 | Deviations below this ramp to 0 |
| `snap` | 1.0 | PID outputs below this → steering=0 |
| `max_rate` | 40 | Max steering change **per lane frame** (~6-10Hz), not per 50Hz tick |
| `throttle` | 28 | Base throttle [0-100], LKA mode |
| `steering_trim` | 22 | Fixed offset added to all steering commands (+ = right); compensates crooked wheels |
| `curve_throttle_min` | 21 | Anti-stall floor: nonzero throttle never goes below this |
| `degraded_threshold_frames` | 10 | Frames without lane before DEGRADED |
| `emergency_threshold_ms` | 1500 | ms in DEGRADED before EMERGENCY_STOP |
| `recovery_threshold_frames` | 15 | Consecutive valid frames to exit EMERGENCY_STOP |
| `emergency_timeout_ms` | 10000 | Deadlock breaker: force EMERGENCY_STOP → DEGRADED after this long |
| `obj_conf_thresh` | 0.60 | Minimum confidence to act on a detected object |
| `collision_dist_m` | 0.30 | Distance (m) triggering full stop |
| `stop_sign_dist_m` | 0.25 | Distance (m) within which a detected `SIGN_STOP` actually stops the car |
| `lane_timeout_ms` / `obj_timeout_ms` / `joy_timeout_ms` | 500 / 1000 / 200 | Watchdog staleness timeouts |
| `acc_enabled` / `acc_target_kmh` / `acc_headway` | 0 / 3.5 / 2 | ACC mode toggle + target speed + headway setting |
| `curve_gain` / `curve_min_factor` / `curve_ema_alpha` / `single_line_factor` | 0.9 / 0.9 / 0.1 / 0.9 | Curve/single-line slowdown tuning |
| `oa_enabled` | 0 | Master OA on/off |
| `oa_wheelbase_m` / `oa_car_width_m` / `oa_margin_m` / `oa_critical_dist_m` / `oa_servo_max_deg` / `oa_throttle_evading` | 0.18 / 0.18 / 0.03 / 0.60 / 15.0 / 25.0 | OA geometry + trigger distance + evading throttle |
| `oa_throttle_{lo,hi}` / `oa_{evade,straight,return}_ms_{lo,hi}` | see file | OA adaptive-timing calibration setpoints |

**The file is read once at startup.** To apply changes: edit `lka_config.conf` (relative to the process's working directory — the systemd service sets `WorkingDirectory` per version, so binary and config always match) and restart the process.

---

## Object Detection Throttle Override

`obj_throttle_limit()` in `adas_state_machine.hpp` — independent of the ADAS state machine, applied whenever driving in AUTONOMOUS (any state that calls `autonomous_driving`).

| Condition | Throttle limit |
|---|---|
| No objects / confidence < `obj_conf_thresh` | 100 (no override) |
| `distance < collision_dist_m` | 0 (full stop) |
| `SIGN_STOP` within `stop_sign_dist_m` | 0 (full stop) — farther away, ignored |
| `SIGN_TL_RED` | 0 (full stop), any distance |
| `SIGN_YIELD` / `SIGN_TL_YELLOW` | `curve_throttle_min` (anti-stall floor, not an absolute cap) |

`SIGN_OBSTACLE` isn't handled here — it feeds Obstacle Avoidance instead (nearest one, by distance, combined with the SRF08 gap from `CtrlStatus_t`; whichever is closer wins).

`throttle_final = min(cfg.throttle, obj_limit, v2i_limit)`, then curve/single-line speed factor applied on top.

---

## V2I / Mobility Scenarios

`V2IFrame` on `/tmp/adas_v2i.sock` — `v2i_throttle_limit()` in `adas_state_machine.hpp`:

| Condition | Throttle limit |
|---|---|
| `priority_active` (emergency vehicle) | 100 — **bypasses** traffic-light/barrier restrictions (object/collision safety still applies separately) |
| `barrier_state` CLOSED or MOVING | 0 |
| `traffic_light_state` RED | 0 |
| `traffic_light_state` YELLOW | `curve_throttle_min` |
| `traffic_light_state` GREEN / UNKNOWN | 100 |

---

## Emergency Priority Vehicle Toggle

New feature: an operator can flag the car as an emergency vehicle to get V2I priority (traffic lights/barriers cleared) at the roadside-scenario level.

- Joystick key `e` → `JoystickMsg::Type::EMERGENCY_TOGGLE` sent over `/tmp/adas_joystick.sock`.
- `adas_manager.cpp` flips an internal `emergency_active` bool and calls `EmergencySender::send()`.
- `EmergencySender` (`emergency_sender.hpp`) fire-and-forget UDP-style UNIX DGRAM `sendto` on `/tmp/adas_emergency.sock` — `"1\n"` / `"0\n"`. If `roadside_emergency_controller.py` isn't running, the send fails silently (ENOENT) and never blocks the control loop.
- `roadside_emergency_controller.py` (`src/mobility_scenarios_src/emergency_priority/`) listens on that socket, flips its own `state.vehicle_mode` (same effect as its own keyboard `t`), drives the real roadside micro:bit devices (traffic light / barrier / streetlight over USB serial → radio), and injects the resulting `V2IFrame` back into ADAS Manager on `/tmp/adas_v2i.sock`.

---

## CAN Protocol (`can_protocol.h` — shared with STM32)

### 0x202 — `CtrlCmd_t` (Manager → STM32, sent every tick, 50Hz)

```
DLC: 8 bytes
data[0]   = mode              (CtrlMode_t: 0=DISABLED 1=MANUAL 2=LKA 3=CC 4=ACC)
data[1]   = steering  (int8, -100..100, trimmed, ALWAYS applied regardless of mode)
data[2]   = throttle  (int8, -100..100, used in MANUAL/LKA only)
data[3:5] = target_speed_cms (uint16 LE, used in CC/ACC only)
data[5]   = headway:4 (HEADWAY_CLOSE/MEDIUM/FAR, ACC only) | aeb_request:1 | reserved:3
data[6]   = counter (rolling 0..14)
data[7]   = CRC-8 (poly=0x07, init=0x00) over bytes 0-6
```

STM32 is the final arbiter of throttle/steering based on `mode`; its own AEB overrides regardless of mode.

### 0x213 — `CtrlStatus_t` (STM32 → Manager, ~10Hz)

```
data[0] = active_mode        (CtrlMode_t actually running on STM32)
data[1] = actual_steering    (int8, applied to servo)
data[2] = actual_throttle    (int8, applied to motor driver)
data[3:5] = current_speed_cms (uint16 LE, from Hall encoder)
data[5:7] = gap_cm            (uint16 LE, from SRF08; 0xFFFF = no lead vehicle)
data[7] = override_reason     (OverrideReason_t)
```

No CRC (validated by range checks instead). Consumed by `status_thread` → feeds `current_speed_cms` (telemetry/logging) and `gap_cm` (OA input, SRF08 fallback for camera detection).

### 0x700 — `Heartbeat_t` (Manager → STM32, 10Hz)

```
data[0] = state (SystemState_t, always SYSTEM_STATE_RUNNING here)
data[1:5] = uptime_ms (uint32 LE)
data[5] = errors (ERROR_FLAG_* bitfield, unused — always 0)
data[6] = mode (DRIVE_MODE_MANUAL / DRIVE_MODE_AUTONOMOUS)
data[7] = CRC-8
```

STM32 failsafes if this (or 0x202) stops arriving.

### 0x001 — `EmergencyStop_t` (Manager → STM32, one-shot on state change)

```
DLC: 8 bytes
data[0]   = active (1=active)
data[1]   = source = 2 (AGL)
data[2:4] = distance_mm = 0 (uint16 LE)
data[4]   = reason (0x10 = LANE_LOSS by default)
data[5:7] = reserved = 0
data[7]   = CRC-8 (poly=0x07, init=0x00) over bytes 0-6
```

Channel: `can1` for all frames.

---

## Input Sockets

All are UNIX DGRAM, bound by the Manager, `SO_RCVTIMEO` so receiver threads can poll `running`.

| Socket | Payload | Producer |
|---|---|---|
| `/tmp/adas_lane.sock` | `LaneFrame` (packed: `lateral_deviation` float + `lane_status` uint8 + 2×`LaneObject` mock/reserved for future MPC) | `INFERENCE_DUAL.py` |
| `/tmp/adas_objects.sock` | `ObjectFrame` (packed: `count` uint8 + up to 4 `DetectedObject{class_id, confidence, distance, theta_cam}`) | `INFERENCE_DUAL.py` |
| `/tmp/adas_v2i.sock` | `V2IFrame` (packed: `traffic_light_state`, `barrier_state`, `priority_active`, `reserved`) | `roadside_emergency_controller.py` |
| `/tmp/adas_joystick.sock` | Text: `J <steer> <throttle>` \| `T` (toggle mode) \| `M` (force manual) \| `A` (force auto) \| `E` (emergency toggle) | `joystick_control.py` |

`receiveLatest()` drains the socket queue and keeps only the most recent frame per read, preventing backlog. Socket recv timeout (120ms for lane/object/v2i, 60ms for joystick) is **not** treated as invalid data — perception runs at ~10.5fps so a timeout between frames is normal; actual staleness is judged by the per-source watchdog timeout in `lka_config.conf` (`lane_timeout_ms`/`obj_timeout_ms`/`joy_timeout_ms`), checked against the frame timestamp every main-loop tick.

---

## KUKSA Bridge

Runs as an asynchronous subprocess (`kuksa_bridge.py`) fed over a thread + bounded queue — the control loop never blocks on network I/O.

- `bridge.pub_lane()` / `pub_objects()` / `pub_mode()` enqueue a formatted string and return immediately.
- A dedicated `bridge_thread` drains the queue and writes to the subprocess pipe (`fputs` + `fflush`).
- Queue capacity: 4 messages — oldest dropped if KUKSA is slow.
- Publish is rate-limited to ≤5Hz and only on new frame data (50Hz publishing was costing ~20% CPU in the bridge + ~50% in the databroker, stolen from inference).
- If `kuksa_bridge.py` crashes or credentials are missing, the control loop is unaffected.

---

## Files

| File | Description |
|---|---|
| `adas_manager.cpp` | Entry point — single-instance lock, thread startup, main 50Hz loop, watchdogs, heartbeat, KUKSA publish scheduling |
| `adas_state_machine.hpp` | `AdasState` transitions, `change_drive_mode()`, `obj_throttle_limit()`, `v2i_throttle_limit()` |
| `drive_control.hpp` | `manual_driving()` / `autonomous_driving()` — LKA + OA + ACC + curve slowdown orchestration, CAN command dispatch |
| `lka_controller.hpp` | `PID` + `LKAController` (EMA, deadband, snap, rate limit, per-frame `dt`) |
| `oa_controller.hpp` | `OAController` — evade/straight/return phase state machine with adaptive timing |
| `config.hpp` | `AdasConfig` struct + `lka_config.conf` loader |
| `shared_state.hpp` | `SharedState` (mutex-guarded) + `StateSnapshot` — cross-thread handoff for lane/object/v2i/joystick/status data |
| `receiver_threads.hpp` | `lane_thread`, `object_thread`, `v2i_thread`, `joystick_thread`, `status_thread` |
| `socket_receiver.hpp` | `SocketReceiver` + `LaneFrame`/`ObjectFrame`/`V2IFrame`/`DetectedObject` structs, `SignClass`/`V2ITrafficLightState`/`V2IBarrierState` enums |
| `joystick_receiver.hpp` | `JoystickReceiver` + `JoystickMsg` — parses `J`/`T`/`M`/`A`/`E` text protocol |
| `can_sender.hpp` | `CanSender` — 0x202 `CtrlCmd_t`, 0x700 `Heartbeat_t`, 0x001 `EmergencyStop_t`, steering trim, CRC-8 |
| `can_receiver.hpp` | `CanReceiver` — filtered read of 0x213 `CtrlStatus_t`, range-validates (no CRC on this frame) |
| `emergency_sender.hpp` | `EmergencySender` — fire-and-forget UDS to `roadside_emergency_controller.py` for the priority-vehicle toggle |
| `kuksa_bridge.hpp` | `KuksaBridge` — async subprocess pipe + bounded queue |
| `kuksa_bridge.py` | Subprocess — reads stdin, publishes to KUKSA via gRPC TLS |
| `telemetry_log.hpp` | `log_tick()` — per-tick console log (state, mode, OA, lane, objects, V2I, ACC/speed) |
| `adas_state.hpp` / `drive_mode.hpp` | `AdasState`, `DriveMode` enums |
| `can_id.h` / `can_protocol.h` | Shared CAN ID / frame-struct definitions (identical copy on STM32 side) |
| `joystick_control.py` | Interactive keyboard client — WASD drive, `t`/`m`/`u` mode control, `e` emergency toggle |
| `lka_config.conf` | Runtime parameters (read once at startup, relative to working directory) |

---

## Build

```bash
g++ -O2 -std=c++17 -o adas_manager adas_manager.cpp -lpthread
```

## Startup

The ADAS Manager must be started **before** the inference script — it creates and binds the sockets `/tmp/adas_lane.sock`, `/tmp/adas_objects.sock`, `/tmp/adas_v2i.sock`, `/tmp/adas_joystick.sock`. Run from the version's own directory so it picks up that version's `lka_config.conf` and `kuksa_bridge.py` (paths are relative to CWD, not the binary).

```bash
./adas_manager
```
