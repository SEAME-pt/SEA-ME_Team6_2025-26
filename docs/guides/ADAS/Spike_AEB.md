# Spike — Automatic Emergency Braking (AEB)

---

## What is AEB?

An ADAS system that detects imminent collisions and brakes automatically without driver intervention. Production systems operate in three phases: alert (FCW), brake assist (DBS), and automatic braking (CIB).

---

## How the Industry Implements AEB

### Sensors

Automotive manufacturers typically use a combination of multiple sensors for obstacle detection:

- **Radar** (77 GHz): primary sensor in most systems, range up to 250m, works in adverse weather conditions (rain, fog, night). VW uses long-range radar (80m) in Front Assist; BMW combines front radar with 4 side radars.
- **Cameras**: complement radar with object classification (pedestrians, cyclists, vehicles). BMW uses a stereo KAFAS camera with three lenses. Camera presence enables greater braking force — VW applies 6 m/s² with camera vs 3.5 m/s² without.
- **LiDAR**: used in premium systems for high-resolution 3D environment mapping.
- **Ultrasonic**: typically used for low speed and parking manoeuvres (short range, ~6m).

### Sensor Fusion

Data from multiple sensors is combined using fusion algorithms (Extended Kalman Filter is the most common) to obtain a more accurate and robust estimate of obstacle position and velocity. The Mercedes-Benz PRE-SAFE monitors over 70 parameters simultaneously (lateral acceleration, steering angle, yaw rate, etc.).

### Response Phases

Production AEB systems follow a cascaded response based on TTC (Time-to-Collision):

1. **FCW — Forward Collision Warning** (~2.6s TTC): visual, audible, or haptic alert to the driver
2. **DBS — Dynamic Brake Support** (~1.6s TTC): if the driver brakes with insufficient force, the system supplements the braking
3. **CIB — Crash Imminent Braking** (~0.6s TTC): full automatic braking if the driver does not react

Emergency deceleration typically reaches 5–10 m/s² (UN ECE Regulation 152 requires a minimum of 5 m/s²).

### Results

Industry data demonstrates AEB effectiveness: the combination of AEB + FCW reduces rear-end collisions by ~50%. 2024 models tested by AAA avoided 100% of collisions at speeds up to 35 mph, compared with 51% in 2017-2018 models.

---

## Our Implementation (PiRacer)

The PiRacer operates at low speed (~1.67 m/s max at full throttle), which simplifies reaction time requirements but maintains the fundamental principles. We use the SRF08 (ultrasonic) as the primary distance sensor + Hall effect speedometer for velocity, with a dedicated AEB task computing TTC, stopping distance, and a kinematic speed profile for smooth deceleration.

### Hardware

| Component | Function | Domain |
|-----------|----------|--------|
| SRF08 Ultrasonic | Primary distance measurement (3cm–6m) | STM32 (I2C, 0xE0) |
| VL53L5CX ToF 8x8 | Secondary distance measurement (8x8 array) | STM32 (I2C, 0x52) |
| Hall Effect Speedometer | Velocity measurement via TIM4 input capture | STM32 (PB6, TIM4 CH1) |
| TB6612FNG | Motor control — forward, reverse, electrical brake | STM32 (I2C, 0x28) |
| Hailo-8 + Camera | Visual detection (complementary, via RPi5) | RPi5 |
| CAN Bus (MCP2515) | Sensor telemetry + motor commands | Both (SPI, 500 kbps) |

---

## Architecture (STM32 + ThreadX)

```
SRF08 (~70ms cycle) ──→ Median Filter (5 samples) ──→ distance_mm ──┐
                                                                      ├──→ AEB Task (50Hz)
Speedometer (TIM4 capture) ──→ speed_mh ─────────────────────────────┘     │
                                                                           ├── TTC calculation
                                                                           ├── Stopping distance model
                                                                           ├── Kinematic speed profile
                                                                           └── Outputs: aeb_speed_limit, aeb_stop_active
                                                                                    │
                                                                              CAN_RX Task
                                                                                    │
                                                                              Motor Control (TB6612FNG)
```

### ThreadX Threads

The system runs multiple concurrent ThreadX threads. Those relevant to AEB:

| Thread | Priority | Period | Function |
|--------|----------|--------|----------|
| AEB Task | 11 | 20ms (50 Hz) | TTC + stopping distance + kinematic speed limit |
| SRF08 Task | 11 | ~70ms (sensor cycle) | Distance measurement + median filter |
| CAN RX Task | 11 | 10ms (polling) | Actuation enforcement — applies speed limits, blocks forward on stop |
| Speed Task | 12 | 100ms | Wheel speed via TIM4 input capture |
| ToF Task | 14 | ~200ms (5 Hz) | VL53L5CX 8x8 distance array |

> **Note:** ThreadX priority 0 is highest. All safety-critical threads run at priority 11 (high). The AEB task shares priority with SRF08 and CAN_RX.

The speedometer (Hall effect encoder, PB6/TIM4 CH1, 18 holes, wheel diameter 66.75mm) provides velocity via TIM4 input capture. The capture delta between pulses is converted to m/h, with a 500ms timeout for zero-speed detection.

---

## AEB State Machine

The AEB task (`task_aeb.c`) implements a 5-state machine based on TTC and stopping distance, not fixed distance thresholds:

```
OFF ──(speed > 0.15 m/s)──→ ARMED ──(TTC < 0.8s || d < d_warn)──→ WARN
                                ↑                                      │
                                └──(TTC > 1.3s && d > d_warn+0.3m)────┘
                                                                       │
                                                            (TTC < 0.5s || d < d_brake)
                                                                       ↓
                              OFF ←──(unlatch after 1s safe)──── LATCHED ←──(stopped 300ms)── BRAKING
```

| State | Condition | Action |
|-------|-----------|--------|
| OFF | Speed < 0.15 m/s (not moving) | No AEB intervention |
| ARMED | Speed >= 0.15 m/s, no risk detected | Kinematic speed profile active (soft limit) |
| WARN | TTC < 0.8s or distance < d_warn | Warning flag set, speed limit from `v = sqrt(2 * a_comfort * d)` |
| BRAKING | TTC < 0.5s or distance < d_brake | Smooth deceleration via kinematic curve (NOT hard stop) |
| LATCHED | Vehicle stopped for >= 300ms while braking | Hard stop (`Motor_Stop()`), forward blocked until obstacle clears |

**Key design decisions:**
- BRAKING does **not** trigger `Motor_Stop()` — it uses a kinematic speed profile `v = sqrt(2 * a * d)` for smooth deceleration
- Only LATCHED triggers a hard stop via `aeb_stop_active = 1`
- Unlatch requires obstacle cleared (> 150mm) for 1 second while stopped

---

## TTC and Stopping Distance Model

### TTC (Time-to-Collision)

```
TTC = (d_filtered - d_offset) / max(v, v_min)
```

Where:
- `d_filtered` = low-pass filtered SRF08 distance (alpha = 0.70)
- `d_offset` = 0.10m (sensor/bumper offset)
- `v` = current vehicle speed (m/s, from `speed_mh / 3600`)
- `v_min` = 0.10 m/s (prevents divide-by-zero)

### Stopping Distance

```
d_stop = v * t_react + v² / (2 * a_brake)
```

Where:
- `t_react` = 0.12s (pipeline latency)
- `a_brake` = 0.6 m/s² (measured deceleration from PWM reduction)

Thresholds are computed dynamically:
- `d_warn = d_stop + 0.30m` (margin for warning)
- `d_brake = d_stop + 0.10m` (margin for braking)

### Kinematic Speed Profile (Soft Braking)

Instead of discrete brake levels, the AEB computes a continuous speed limit:

```
v_limit = sqrt(2 * a_comfort * d_effective)
throttle_limit_pct = (v_limit / v_max) * 100
```

Where:
- `a_comfort` = 0.15 m/s² (conservative deceleration)
- `v_max` = 1.67 m/s (measured max speed at 100% throttle)
- `d_effective` = filtered distance - sensor offset

This gives a smooth deceleration curve: as distance decreases, the allowed throttle percentage decreases proportionally. An asymmetric slew-rate limiter prevents sudden jumps (max +5%/step increase, -10%/step decrease).

---

## Tunable Parameters

All AEB parameters are defined in `task_aeb.c` as a `const AebParams` struct:

| Parameter | Value | Description |
|-----------|-------|-------------|
| `d_offset_m` | 0.10 m | Sensor/bumper offset subtracted from raw distance |
| `v_min_mps` | 0.10 m/s | Minimum speed used in TTC (prevents div/0) |
| `t_react_s` | 0.12 s | Reaction/actuation delay |
| `a_brake_mps2` | 0.6 m/s² | Realistic deceleration from PWM reduction |
| `a_comfort_mps2` | 0.15 m/s² | Conservative deceleration for kinematic curve |
| `ttc_warn_s` | 0.8 s | TTC threshold to enter WARN state |
| `ttc_brake_s` | 0.5 s | TTC threshold to enter BRAKING state |
| `margin_warn_m` | 0.30 m | Extra distance margin added to d_stop for warning |
| `margin_brake_m` | 0.10 m | Extra distance margin added to d_stop for braking |
| `speed_arm_mps` | 0.15 m/s | AEB only arms above this speed (~540 m/h) |
| `speed_stop_mps` | 0.10 m/s | Speed below this counts as "stopped" |
| `stop_hold_ms` | 300 ms | Must remain stopped this long to latch |
| `lp_alpha` | 0.70 | Low-pass filter coefficient for distance |
| `safe_unlatch_dist_m` | 0.15 m | Obstacle must be this far to unlatch |
| `safe_unlatch_hold_ms` | 1000 ms | Must remain safe this long to unlatch |
| `v_max_mps` | 1.67 m/s | Measured max speed at 100% throttle |
| `limit_slew_up_pct` | 5% | Max throttle increase per 20ms step |
| `limit_slew_down_pct` | 10% | Max throttle decrease per 20ms step |

---

## Sensor Filtering

**SRF08**: Median filter (5 samples, insertion sort) to reject ultrasonic spikes. Light threshold (>= 2) used to validate readings — readings with `light == 0 && distance == 0` are treated as "no echo" and rejected.

**AEB low-pass**: Applied on top of the median-filtered SRF08 output. First-order IIR: `d_f = d_f + alpha * (d_raw - d_f)` with alpha = 0.70.

**Speedometer**: TIM4 input capture measures time between encoder pulses. Speed = 0 if no pulse for 500ms (timeout detection). Wheel circumference = pi * 66.75mm.

---

## Actuation Enforcement (CAN RX Task)

The CAN RX task reads AEB outputs from the shared `VehicleState` and enforces them:

1. **Global throttle cap**: `AEB_MAX_THROTTLE_PCT = 60%` — limits maximum throttle regardless of AEB state
2. **Speed limit cascade**: `limit = min(AEB_MAX_THROTTLE_PCT, srf08_speed_limit, aeb_speed_limit)` — tightest limit wins
3. **Hard stop**: If `aeb_stop_active == 1` or `emergency_stop_active == 1`, forward motion is blocked and `Motor_Stop()` is called
4. **Reverse allowed**: Even during AEB stop, reverse motion is permitted (to back away from obstacle)
5. **Proactive enforcement**: Between CAN commands, the CAN RX task actively reduces motor PWM to match the AEB kinematic limit. Below 15% PWM, it uses `Motor_Stop()` (electrical brake) instead of low-PWM coasting

---

## Fault Handling

The AEB task validates sensor data freshness before making decisions:

- SRF08 data must be < 200ms old (`max_srf_age_ms`)
- Speed data must be < 200ms old (`max_speed_age_ms`)
- SRF08 `valid` flag must be set (I2C read successful)

**Fault policy:**
- If sensors become invalid while BRAKING or LATCHED: stay LATCHED and maintain stop request (fail-safe bias)
- If sensors become invalid while OFF/ARMED/WARN: go OFF and clear stop request

---

## CAN Communication

### Sensor Telemetry (STM32 -> AGL)

| CAN ID | Name | Payload | Rate |
|--------|------|---------|------|
| 0x423 | SRF08_Distance | distance_mm (2B), light_level (1B), gain (1B), range (1B), reserved (2B), status (1B) | ~15 Hz |
| 0x422 | ToF_Distance | min_distance_mm (2B), nearest_zone (1B), target_status (1B), detection_count (1B), reserved (2B), status (1B) | 5 Hz |
| 0x403 | Wheel_Speed | rpm (2B), total_pulses (4B), direction (1B), status (1B) | 10 Hz |
| 0x210 | Motor_Status | actual_throttle (1B), actual_steering (1B), motor_current_ma (2B), driver_temp (1B), pwm_duty (1B), counter (1B), crc (1B) | 10 Hz |

### Commands (AGL -> STM32)

| CAN ID | Name | Payload | Description |
|--------|------|---------|-------------|
| 0x001 | Emergency_Stop | active (1B), source (1B), distance_mm (2B), reason (1B), reserved (2B), crc (1B) | Bi-directional emergency stop |
| 0x200 | Motor_Cmd | throttle (1B), steering (1B), flags (1B), mode (1B), reserved (2B), counter (1B), crc (1B) | Throttle/steering commands |
| 0x500 | Joystick | steering (2B LE), throttle (2B LE) | Direct joystick input |

All CAN frames use CRC-8 validation. Emergency stop sources: 0=ToF, 1=SRF08, 2=AGL, 3=Manual, 4=Watchdog.

> **Note:** AEB state is NOT transmitted over CAN as a dedicated frame. AEB outputs (speed limit, stop flag) are enforced locally on the STM32 via the shared `VehicleState` struct and applied by the CAN RX task during motor command processing.

---

## Acceptance Criteria (EXPECT-L0-27)

- System reaction time (detection to braking start): **< 100 ms** (AEB loop runs at 50 Hz = 20ms)
- Stopping distance from hard brake activation: **< 5 cm**
- Inference latency p99 (if camera involved): **< 50 ms**
- False positive rate: **< 5%**

---

## Risks

| Risk | Mitigation |
|------|------------|
| SRF08 erratic readings (spikes, no-echo) | Median filter (5 samples) + light validation + low-pass in AEB |
| State oscillation | TTC-based hysteresis: WARN->ARMED requires TTC > 1.3s AND distance > d_warn + 0.3m |
| Hailo inference latency > 50ms | AEB primary sensor is SRF08/STM32; camera is complementary |
| Sensor data becomes stale | Freshness windows (200ms) + fail-safe policy (latch on stale data during braking) |
| Motor dead zone at low PWM | Below 15% PWM, use `Motor_Stop()` (electrical brake) instead of coasting |

---

## Open Decisions

- Sensor priority: SRF08-only vs SRF08+Camera fusion
- Detection model: YOLOv8n vs YOLOv8s vs YOLOv8m
- Whether to add a dedicated AEB CAN frame for RPi5 telemetry/logging

---

## Done When

- [x] State machine designed and validated (5-state: OFF/ARMED/WARN/BRAKING/LATCHED)
- [x] TTC + stopping distance model implemented with tunable parameters
- [x] Kinematic speed profile for smooth deceleration
- [x] CAN protocol defined (0x001, 0x200, 0x422, 0x423, 0x403)
- [ ] TSF requirements mapped to acceptance criteria

---

*Document version 2.0 · SEA:ME Team 6 · March 2026*
