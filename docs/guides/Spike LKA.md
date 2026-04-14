# Spike — Lane Keeping Assist (LKA)

> **Assignees**: Ruben + David · **Module**: 02 — ADAS · **TSF**: EXPECT-L0-17
>
> **Status**: Research/design only — **not implemented**. Per the ADAS module roadmap, LKA was planned for Sprint 11 (weeks 5-6). This document captures the spike research and proposed design.

---

## What is LKA?

An ADAS system that detects lane boundaries and automatically corrects steering when the vehicle begins to leave the lane without the driver signalling.

There are three levels of lateral assistance:

| System | What it does | Type |
|--------|-------------|------|
| **LDW** (Lane Departure Warning) | Warns the driver | Passive |
| **LKA** (Lane Keeping Assist) | Corrects steering after deviation | Reactive |
| **LCA** (Lane Centering Assist) | Continuously keeps the car centred | Proactive |

The proposed PiRacer implementation targets **LDW + LKA**: detect lanes, warn, and correct steering.

---

## How the Industry Implements LKA

### Sensors and Detection

Production systems (Honda LKAS, Toyota Lane Tracing Assist, Ford Lane-Keeping System) use a **front-facing camera** mounted near the rear-view mirror with a ~40° field of view. The camera captures the road ahead and computer vision algorithms (typically CNNs) detect lane markings. Modern systems achieve >97% accuracy on benchmarks like TuSimple.

Some manufacturers complement the camera with LiDAR or infrared sensors to improve detection in low-visibility conditions, but the camera remains the primary and mandatory sensor.

### Control Logic

The system continuously computes two key values:
- **Lateral offset**: distance from the car centre to the lane centre
- **TLC (Time to Line Crossing)**: estimated time until the car crosses the lane marking

When TLC drops below a threshold (and the turn indicator is not active), the system intervenes. Honda LKAS, for example, provides up to 80% of the steering torque needed to keep the car in the lane. Intervention is always smooth — the driver can easily override the system.

### Known Limitations

Industry LKA systems depend on visible markings — roads with faded markings, snow coverage, or no markings reduce effectiveness. Very tight curves and extreme lighting conditions are also problematic. According to an IIHS study, 49% of drivers had the system turned off, partly due to false positives.

---

## Proposed Implementation (PiRacer)

Use the IMX708 camera with inference on the Hailo-8 (26 TOPS) to detect lanes, and send steering correction commands to the STM32 via CAN to actuate the servo.

### Hardware

| Component | Function | Domain |
|-----------|----------|--------|
| IMX708 Camera | Track frame capture | RPi5 |
| Hailo-8 (26 TOPS) | Lane detection inference | RPi5 |
| MG996R Servo | Steering actuator (PWM, TIM1_CH1) | STM32 |
| CAN Bus (MCP2515) | Steering commands | Both (SPI, 500 kbps) |

---

## Proposed Inference Pipeline

```
Camera (30fps) → rpicam-vid (MJPEG) → Python (Queue maxsize=1) → Hailo-8 (UFLDv2) → Lane coords
                                                                                          │
                                                                        offset + yaw calc ←┘
                                                                              │
                                                                    LKA State Machine → CAN → STM32 → Servo
```

Dedicated camera thread with `Queue(maxsize=1)` — discards stale frames to avoid accumulated latency. This pattern is already proven in the YOLO object detection pipeline (see Hailo-8 integration doc).

---

## Model: UFLDv2 (Ultra Fast Lane Detection v2)

Recommended model — pre-compiled HEF available in the Hailo Model Zoo, with an official example in the `hailo-ai/hailo-apps` repo.

Instead of classifying each pixel (segmentation), UFLDv2 divides the image into horizontal rows and predicts the x-position of the lane in each row. This is much faster and produces lane coordinates directly.

**Fallback**: If UFLDv2 does not generalise to the miniature track (high risk — it was trained on real roads), the fallback plan is YOLOv8-seg with a custom dataset of our track.

```bash
# Transfer HEF to the RPi5 (no internet on device)
scp ufld_v2.hef root@10.21.220.192:/data/models/
```

---

## Proposed Key Calculations

**Lane offset** (distance to lane centre):
```
lane_center = (left_lane_x + right_lane_x) / 2
offset = lane_center - image_center
```

**Yaw angle** (angle relative to the lane):
```
yaw = arctan(average slope of lanes between image top and bottom)
```

**TLC** (time to line crossing):
```
TLC = lateral_distance_to_marking / lateral_velocity
```

---

## Proposed State Machine

```
OFF ──(lanes detected)──→ CENTERED ──(TLC < threshold)──→ WARNING (LDW) ──(TLC < critical)──→ CORRECTING (LKA)
                               ↑                                                                      │
                               └────────────────────(offset ≈ 0)──────────────────────────────────────┘
```

| State | Condition | Action |
|-------|-----------|--------|
| OFF | Lanes not detected (>2s) or system disabled | No intervention |
| CENTERED | Offset < threshold | Active monitoring |
| WARNING | TLC < threshold | Visual alert on HMI |
| CORRECTING | TLC < critical | Steering correction via servo |

**Rule**: If AEB is active, LKA is suspended — AEB always has priority. This follows the existing AEB implementation where `aeb_stop_active` or `emergency_stop_active` blocks forward motion in `task_can_rx.c`.

---

## Proposed CAN Communication

> **Note:** These CAN IDs are proposed and do NOT exist in the current firmware. The existing CAN ID map (`can_id.h`) has no lane-related messages. Implementation would require adding these IDs and corresponding handlers in `task_can_rx.c` on the STM32 side.

| CAN ID | Name | Payload | Direction |
|--------|------|---------|-----------|
| 0x110 | LKA_Status | state (1B), offset (2B signed), yaw (2B signed) | RPi5 → STM32 |
| 0x111 | LKA_SteerCmd | steering_correction (2B signed), confidence (1B) | RPi5 → STM32 |

### Integration with Existing Motor Control

The STM32 servo is currently controlled only via joystick/motor commands (CAN IDs 0x200 and 0x500). The steering mapping is:

```c
// Current implementation in task_can_rx.c
uint8_t servo_angle = (uint8_t)((100 - steering) * 180 / 200);
Servo_SetAngle(servo_angle);
```

LKA steering corrections would need to be blended with or override manual steering input, similar to how AEB speed limits override throttle commands.

---

## Risks

| Risk | Mitigation |
|------|------------|
| UFLDv2 does not detect lanes on the miniature track | Fallback YOLOv8-seg with custom dataset |
| Total latency (camera → servo) > 200ms | Queue(maxsize=1), reduce resolution |
| Servo oscillations | Limit steering rate of change (slew rate) |
| Inconsistent lighting | Image normalisation |
| Integration with AEB priority logic | Use existing `aeb_stop_active` flag to suspend LKA |

---

## Open Decisions

- Model: UFLDv2 out-of-box vs YOLOv8-seg custom
- Controller: PID vs simple proportional
- Custom dataset: needed or not (depends on testing UFLDv2 first)
- Input resolution: original vs adapted (FPS vs precision)
- CAN protocol: new IDs (0x110/0x111) or reuse motor command (0x200) with mode flag

---

## Done When

- [ ] UFLDv2 HEF tested on the Hailo-8 with images from the track
- [ ] Decision documented: UFLDv2 works or needs fallback
- [ ] Inference pipeline designed
- [ ] CAN protocol for LKA defined and added to `can_id.h`
- [ ] Integration with AEB defined (priorities)
- [ ] Steering correction handler added to `task_can_rx.c`

---

*Document version 2.0 · SEA:ME Team 6 · March 2026*
