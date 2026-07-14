# Assisted Driving Mode — Design

## Context

`ADAS-Manager` (`src/ADAS-Manager/`) is a single-TU C++17 control loop (20ms tick,
`adas_manager.cpp`) that drives the vehicle in two modes today: `MANUAL`
(joystick steering + throttle) and `AUTONOMOUS` (LKA steering + ACC/throttle,
with sign/traffic-light/collision throttle limiting always on).

The HMI (`src/hmi/`, Qt/QML) got 4 ADAS feature toggles merged into
`development` in parallel (PR #393, `feat/qt/adas-implementation`):

- `Vehicle.ADAS.LaneKeepAssist.IsEnabled` (LKA)
- `Vehicle.ADAS.CruiseControl.IsEnabled` (CC)
- `Vehicle.ADAS.AEB.IsEnabled` (AEB)
- `Vehicle.ADAS.ObjectDetection.IsEnabled` (TSR)

Wired end-to-end on the HMI side: QML `SettingsScreen.qml` → `ADASProvider::
triggerUpdateADASFeature` → `Writer::writeSignal` → KUKSA (gRPC v2, TLS+JWT).
ADAS-Manager currently has no path to receive these — `KuksaBridge`
(`kuksa_bridge.hpp` + `kuksa_bridge.py`) only *publishes* (car → KUKSA) via a
one-directional `popen(cmd, "w")` subprocess pipe. This design adds the
subscribe path and the driving logic that consumes it.

## Goal

Replace the binary MANUAL/AUTONOMOUS split with a driving mode driven by 4
independent live feature flags, subscribed from KUKSA in real time, without
blocking the 20ms control loop.

## Flag semantics

| Flag | Controls | Applies to |
|---|---|---|
| LKA | steering source: LKA-computed vs joystick | always |
| CC  | throttle base source: ACC/`cfg.throttle` calc vs joystick | always |
| AEB | hard-stop on imminent collision (`collision_dist_m` check) | overlay on top of whichever throttle is active (joystick or CC) |
| TSR | sign/traffic-light throttle limiting (`obj_throttle_limit`/`v2i_throttle_limit`) | overlay on top of whichever throttle is active (joystick or CC) |

Confirmed out of scope: OA (obstacle-avoidance evasive steering maneuver,
`OAController`) stays gated by the existing static `cfg.oa_enabled` config
value — not wired to any live KUKSA flag. AEB here means only the hard
collision-distance stop, not OA.

## Drive mode (label only)

`DriveMode` becomes a pure function of how many of the 4 flags are on,
recomputed every tick — no separate lock/latch state:

- 0 flags on → `MANUAL`
- 1–3 flags on → `ASSISTED`
- 4 flags on → `AUTONOMOUS`

The HMI is responsible for preventing the user from unchecking a flag while
in `AUTONOMOUS` (greyed-out toggles); ADAS-Manager does not enforce this — if
a flag flips off while all 4 were on, the mode simply demotes to `ASSISTED`
next tick. This is safe by construction and needs no extra code.

Default at boot / before any KUKSA value has been received: all 4 flags
`false` → `MANUAL`. Matches the existing fail-safe pattern in the codebase
(e.g. `lane_valid`/`joy_valid` default false until a real message arrives).

## Required refactor: split `obj_throttle_limit`

`obj_throttle_limit` (`drive_control.hpp:20-33`) today conflates two
concerns in one function: the collision-distance hard stop (`distance <
collision_dist_m → return 0`) and the sign/traffic-light-class limit
(STOP/red→0, YIELD/yellow→`min_throttle`). Since AEB and TSR are now
independently toggleable, this must split into two functions:

- `collision_hard_stop_limit(obj, obj_valid, collision_dist_m)` — the
  distance check only, gated by `aeb_enabled`.
- `sign_throttle_limit(obj, obj_valid, conf_thresh, min_throttle)` — the
  class-based STOP/YIELD/red/yellow checks, gated by `tsr_enabled`.

`v2i_throttle_limit` stays as-is and is fully gated by `tsr_enabled` (V2I
traffic-light/barrier state is sign/light information, same category as
TSR).

## Driving computation (replaces `manual_driving`/`autonomous_driving` split)

Steering and throttle become independently composed instead of picked as a
pair per mode:

```
steering = lka_enabled ? lka.compute(lane.lateral_deviation, dt) : joy_steering

throttle_base = cc_enabled
    ? (cfg.acc_enabled ? <ACC target-speed calc> : cfg.throttle)
    : joy_throttle

throttle_limit = 100
if (aeb_enabled) throttle_limit = min(throttle_limit, collision_hard_stop_limit(...))
if (tsr_enabled) throttle_limit = min(throttle_limit, sign_throttle_limit(...), v2i_throttle_limit(...))

throttle = clamp(throttle_base, 0, throttle_limit)
```

STM32 firmware finding (`stm/Core/Src/tasks/task_can_rx.c:297-304`): steering
is applied unconditionally from `cmd->steering` regardless of `mode` — mode
only affects throttle handling. `CTRL_MODE_MANUAL` and `CTRL_MODE_LKA` treat
throttle identically (`cmd->throttle` passthrough). This means **no new CAN
control mode and no STM32 firmware change is needed**. CAN mode sent each
tick is simply: `CTRL_MODE_ACC` if `cc_enabled && cfg.acc_enabled`, else
`CTRL_MODE_LKA` (covers both pure-MANUAL-equivalent and mixed-assist cases —
functionally identical to `CTRL_MODE_MANUAL` on the STM32 side).

`adas_state_machine` (INIT/ACTIVE/DEGRADED/EMERGENCY_STOP, lane-loss
recovery/timeout logic) keeps running whenever `lka_enabled` is on, same as
it does for `AUTONOMOUS` today — it's about lane-tracking health, not about
the mode label.

## KUKSA subscribe architecture

New, separate Python subprocess `kuksa_subscriber.py`, launched via
`popen(cmd, "r")` (read-only pipe — avoids needing a custom bidirectional
fork/pipe, since standard `popen()` can't do both directions). It:

- Opens its own gRPC `Subscribe` stream (same pattern as `ReaderWorker` in
  `src/shared/backend/core/reader.cpp`, using `kuksa.val.v2`) to the 4 paths.
- On each update, writes one line to stdout, e.g. `F LKA 1\n`, and flushes.
- Runs independently of the existing publish bridge (`kuksa_bridge.py`) —
  that one is untouched, still `popen(cmd, "w")`.

ADAS-Manager side, mirroring the existing `receiver_threads.hpp` pattern
(one thread per external source, writes into `SharedState` under `mtx`):

- New `feature_flags_thread(SharedState&)` in `receiver_threads.hpp`, reads
  lines from the subscriber subprocess's stdout via `fgets`, parses `F <NAME>
  <0|1>`, updates `state.lka_enabled` / `cc_enabled` / `aeb_enabled` /
  `tsr_enabled` under the mutex.
- `SharedState`/`StateSnapshot` gain the 4 bools (default `false`).
- `KuksaBridge` (or a small sibling class) owns launching/joining this second
  subprocess, alongside the existing publish one.

**Required fix**: `kuksa_bridge.py` currently publishes
`Vehicle.ADAS.ObjectDetection.IsEnabled = True` once at startup
(`_pub_bool(stub, meta, _P_OBJ_ENABLED, True)`, line 81). That path is now a
real user-writable actuator (TSR toggle) owned by the HMI. This startup
publish must be removed — otherwise every ADAS-Manager restart stomps the
user's TSR choice back to `True`.

## Telemetry / logging

`log_tick` (`telemetry_log.hpp`) currently prints `MANUAL`/`AUTO  `. Extend to
print the derived mode label (`MANUAL`/`ASSISTED`/`AUTONOMOUS`) and, at least
in `ASSISTED`, which of the 4 flags are active, so a log line makes it
obvious why the car is behaving as it is (e.g. `LKA CC --- TSR`).

`bridge.pub_mode` (`kuksa_bridge.hpp`/`.py`) publishes
`Vehicle.ADAS.DrivingMode` as `MANUAL`/`AUTONOMOUS` — extend to also send
`ASSISTED` so the HMI dashboard reflects it.

## Testing

Manual on-bench verification (no simulator in this repo for ADAS-Manager):

1. Toggle each of the 4 KUKSA flags independently via the HMI (or
   `kuksa-client` CLI against the same broker) and confirm via ADAS-Manager
   logs that `SharedState` flags update within one tick and the mode label
   changes correctly (0→MANUAL, 1-3→ASSISTED, 4→AUTONOMOUS).
2. With `LKA=on, CC=off`: confirm steering follows the lane, throttle follows
   the joystick.
3. With `LKA=off, CC=on`: confirm steering follows the joystick, throttle
   follows ACC/`cfg.throttle`.
4. With `AEB=on` and an object placed inside `collision_dist_m`: confirm
   throttle forced to 0 regardless of joystick input, in both CC on and off.
5. With `TSR=off`: confirm STOP sign / red light no longer limits throttle at
   all (per confirmed semantics — TSR off means fully ignored, not just the
   soft yellow/yield limit).
6. Restart `adas_manager` with TSR previously set to `false` via HMI; confirm
   it stays `false` after restart (regression check for the startup-publish
   fix).
