# Obstacle Avoidance — Implementation in ADAS Manager

## Overview

The Obstacle Avoidance (OA) module runs inside `adas_manager` (C++) at 50 Hz alongside the Lane Keeping Assist (LKA). It detects obstacles in the vehicle's path via camera and executes a time-based evasion maneuver, overriding LKA steering and throttle for the duration of the maneuver.

---

## System Architecture

```
INFERENCE_DUAL_THETA.py  (Python, camera + YOLO)
         │
         │  ObjectFrame  (UNIX DGRAM socket /tmp/adas_objects.sock)
         ▼
   object_thread  ──→  SharedState
         │
         ▼  (50 Hz main loop)
   autonomous_driving()
         │
         ├── OAController::adapt_timings(throttle)   ← adaptive timing
         ├── OAController::step()                    ← state machine
         │
         ├── [OA active]  →  override steering + throttle
         └── [OA idle]    →  LKAController::compute()
                                    │
                                    ▼
                              CAN Bus → STM32
```

---

## Detection Pipeline

### Object data (Python → C++)

`INFERENCE_DUAL_THETA.py` runs YOLO on each camera frame and sends an `ObjectFrame` struct via UNIX DGRAM socket:

```
ObjectFrame {
    uint8   count
    N × DetectedObject {
        uint8   class_id     (SignClass enum)
        float32 confidence
        float32 distance_m
        float32 theta_cam    (degrees: 0=straight, +=right, -=left)
    }
}
```

Only objects with `class_id == SIGN_OBSTACLE` (YOLO classes 8=car, 11=obstacle) are forwarded to the OA controller.

### Nearest obstacle selection

```cpp
float nearest_dist_m = 9999.0f;
float nearest_theta  = 0.0f;
bool  cam_valid_oa   = false;

for each object in ObjectFrame:
    if class_id == SIGN_OBSTACLE && distance < nearest_dist_m:
        nearest_dist_m = distance
        nearest_theta  = theta_cam
        cam_valid_oa   = true
```

---

## OA State Machine

### Phases

```
        ┌──────────────────────────────────────────────────────┐
        │                                                      │
        ▼                                                      │
      IDLE  ──[obstacle in path]──→  EVADE  ──→  STRAIGHT  ──→  RETURN
        ▲                                                      │
        └──────────────────────────────────────────────────────┘
```

| Phase    | Steering         | Timer        | Next     |
|----------|------------------|--------------|----------|
| IDLE     | —                | —            | EVADE (on trigger) |
| EVADE    | `evade_steering` | `evade_ms`   | STRAIGHT |
| STRAIGHT | `0`              | `straight_ms`| RETURN   |
| RETURN   | `-evade_steering` | `return_ms` | IDLE     |

All timers are pure time-based — obstacle detection is irrelevant once the maneuver starts.

### Trigger condition (IDLE only)

```
clearance   = car_width_m / 2 + margin_m
y_obj       = distance × sin(theta_cam)        ← lateral position of obstacle
cam_in_path = (distance < critical_dist_m) AND (|y_obj| < clearance)
```

If `cam_in_path` is true → maneuver starts.  
If obstacle is closer than the computed arc allows → `BLOCKED` (car stops).

### Evasion angle (Ackermann geometry)

The steering angle is computed once at trigger time:

```
1. Target waypoint (pass obstacle with clearance):
   y_alvo = y_obj - clearance    (obstacle right → pass left)
   y_alvo = y_obj + clearance    (obstacle left  → pass right)

2. Turning radius to reach waypoint from origin:
   R = (x_obj² + y_alvo²) / (2 × |y_alvo|)

3. Ackermann steering angle:
   α = arctan(wheelbase / R)

4. Normalise to servo range:
   normalised = (α / servo_max_deg) × 100%

   |normalised| > 100%  →  BLOCKED (turn physically impossible)
```

The RETURN phase uses `-evade_steering` (symmetric, restores original heading).

---

## Adaptive Timing

Maneuver duration scales with vehicle speed (approximated by throttle %) via linear interpolation between two calibrated setpoints:

```
factor     = clamp((throttle - throttle_lo) / (throttle_hi - throttle_lo), 0, 1)
evade_ms   = evade_ms_lo   + (evade_ms_hi   - evade_ms_lo)   × factor
straight_ms = straight_ms_lo + (straight_ms_hi - straight_ms_lo) × factor
return_ms  = return_ms_lo  + (return_ms_hi  - return_ms_lo)  × factor
```

Timings are only updated while OA is in **IDLE** — mid-maneuver values are frozen.

### Calibrated setpoints (default)

| Throttle | evade_ms | straight_ms | return_ms |
|----------|----------|-------------|-----------|
| 20%      | 1700     | 700         | 2500      |
| 28%      | 1000     | 200         | 1800      |

All values are configurable in `lka_config.conf` without recompiling.

---

## Integration with LKA

During an OA maneuver the vehicle intentionally leaves the lane. To prevent conflicts:

1. **LKA frozen** — `lka.last_steering()` is used instead of `lka.compute()`, pausing the PID integrator.
2. **LKA reset** — when the maneuver ends (EVADING → NORMAL transition), `lka.reset()` clears the accumulated integral.
3. **ADAS state machine frozen** — lane loss during OA does not increment `degraded_frames` (the state machine receives `lane_ok = true` while `oa.state() != NORMAL`), preventing DEGRADED / EMERGENCY_STOP.

```cpp
bool oa_maneuver_active = (oa.state() != OAState::NORMAL);
adas_state_machine(lane_ok || oa_maneuver_active, ...);
```

---

## Command Priority

When OA is active, it overrides the final CAN command:

```
if (oa_active):
    steering = oa_res.steering
    throttle = oa_res.throttle       ← throttle_evading, ignores obj_throttle_limit
elif (oa_res.state == BLOCKED):
    send CTRL_MODE_DISABLED          ← car stops
else:
    steering = lka.compute(...)
    throttle = min(cfg.throttle, obj_throttle_limit)
```

`BLOCKED` takes priority over normal LKA. `EVADING` takes priority over `obj_throttle_limit` (stop signs, speed limits).

---

## Configuration Reference

All parameters loaded from `lka_config.conf` at startup:

| Parameter           | Default | Description                                      |
|---------------------|---------|--------------------------------------------------|
| `oa_enabled`        | 1       | Enable/disable OA (0 = LKA only)                |
| `oa_wheelbase_m`    | 0.18    | Distance between axles (metres)                  |
| `oa_car_width_m`    | 0.18    | Vehicle width (metres)                           |
| `oa_margin_m`       | 0.03    | Lateral safety margin (metres)                   |
| `oa_critical_dist_m`| 0.40    | Trigger distance (metres)                        |
| `oa_servo_max_deg`  | 15.0    | Maximum servo angle (degrees)                    |
| `oa_throttle_evading`| 25.0   | Throttle during maneuver (%)                     |
| `oa_throttle_lo`    | 20.0    | Low throttle calibration setpoint                |
| `oa_evade_ms_lo`    | 1700    | EVADE duration at low throttle (ms)              |
| `oa_straight_ms_lo` | 700     | STRAIGHT duration at low throttle (ms)           |
| `oa_return_ms_lo`   | 2500    | RETURN duration at low throttle (ms)             |
| `oa_throttle_hi`    | 28.0    | High throttle calibration setpoint               |
| `oa_evade_ms_hi`    | 1000    | EVADE duration at high throttle (ms)             |
| `oa_straight_ms_hi` | 200     | STRAIGHT duration at high throttle (ms)          |
| `oa_return_ms_hi`   | 1800    | RETURN duration at high throttle (ms)            |

---

## Key Files

| File | Role |
|------|------|
| `src/ADAS-Manager/oa_controller.hpp` | OA state machine, geometry, adaptive timing |
| `src/ADAS-Manager/adas_manager.cpp` | Main loop integration, LKA/ADAS coordination |
| `src/ADAS-Manager/socket_receiver.hpp` | ObjectFrame struct definition |
| `src/ADAS-Manager/lka_config.conf` | Runtime configuration |
| `src/lane-detection/inference/INFERENCE_THETA_V5.py` | YOLO inference + socket sender |
| `src/ADAS-Manager/socket_sender.py` | Python-side socket serialisation |
