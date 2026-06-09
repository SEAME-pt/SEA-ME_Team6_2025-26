# Lane Detection — SEAME Team 6

Real-time lane detection pipeline for the PiRacer, using UFLDv2 (ResNet-34) with the Hailo-8 NPU and CPU post-processing on the RPi5.

---

## Structure

```
lane_detection/
├── config.py          # All constants and configurable parameters
├── postprocess.py     # CPU post-processing, lane decoding, smoothing, lateral deviation
├── visualization.py   # Lane drawing, drivable area and telemetry overlay
└── inference.py       # Main pipeline — entry point
```

---

## How to run

```bash
# Basic (60 seconds)
python3 inference.py

# Custom duration
python3 inference.py 120

# Save annotated video
python3 inference.py 60 --save

# With MJPEG stream (open in browser at http://10.21.220.191:8081/)
python3 videostream.py inference.py 60 --save
```

---

## Pipeline architecture

```
[IMX708 CSI]
     │ rpicam-vid MJPEG → CameraBroker
     ▼
[Frame BGR 820×616]
     │ preprocess: resize 800×400 → bottom crop → RGB UINT8
     ▼
[Hailo-8 NPU] — ResNet-34 backbone + /pool/Conv
     │ output: conv37 (10, 25, 8) UINT8
     ▼
[CPU — postprocess]
     │ 1. Dequantization: (uint8 - 157) × 0.2719
     │ 2. Transpose HWC→CHW: (10,25,8) → (8,10,25) → flatten (2000,)
     │ 3. FC1: W1 (2048×2000) @ x + b1 → ReLU
     │ 4. FC2: W2 (39576×2048) @ x + b2
     │ 5. Split: loc_row, loc_col, exist_row, exist_col
     ▼
[decode_lanes]
     │ local softmax per anchor → (x, y) coordinates in 820×616 frame
     ▼
[TemporalLaneSmoother + smooth_lanes]
     │ temporal filtering (history=7, min_hits=4) + spatial moving average
     ▼
[calc_lateral_deviation]
     │ normalised deviation [-1.0, +1.0] with camera offset compensation
     ▼
[KUKSA.val — gRPC v2]
     Vehicle.ADAS.LaneKeepAssist.LateralDeviation (float)
     Vehicle.ADAS.LaneKeepAssist.LaneStatus       (string)
```

---

## Model

| Parameter | Value |
|---|---|
| Architecture | UFLDv2 ResNet-34 |
| Training dataset | TuSimple + SEAME track finetune |
| HEF | `tusimple_res34_cut_v2.hef` |
| Split point | `/pool/Conv` — Hailo runs backbone, CPU runs FC layers |
| Model input | 800×320 RGB UINT8 |
| Hailo output | `conv37` (10, 25, 8) UINT8 |
| Calibration SNR | 27.58 dB |
| Hailo latency | ~11ms |
| CPU latency | ~30ms |
| Total FPS | ~22 FPS |

### Why split at `/pool/Conv`?

The Hailo-8 does not support FC layers with more than 2048 output channels. The TuSimple model has FC layers with 39576 outputs — impossible to run on the NPU. The solution is to split the model before the FC layers, run the backbone on the Hailo (~11ms) and the FC layers on the CPU using numpy/OpenBLAS (~30ms), totalling ~41ms vs. ~107ms for the full model on the Hailo.

---

## Key parameters (`config.py`)

| Parameter | Value | Description |
|---|---|---|
| `EXIST_THRESHOLD` | 0.5 | Minimum score to accept an anchor as a lane point |
| `EXIST_THRESHOLD_TOP` | 0.8 | Minimum score for top anchors (row_idx ≤ 20) |
| `TEMPORAL_HISTORY` | 7 | Number of frames kept in the temporal smoother |
| `TEMPORAL_MIN_HITS` | 4 | Minimum detections in history to emit a point |
| `SPATIAL_WINDOW` | 9 | Moving average window for spatial smoothing |
| `CAMERA_OFFSET_NORM` | 0.142 | Compensation for physical camera misalignment |
| `LATERAL_DEVIATION_N` | 10 | Number of bottom points used to compute lateral deviation |

### Adaptive threshold per anchor

Top-of-image anchors (row_idx ≤ 20, corresponding to the furthest zone from the vehicle) are less reliable after INT8 quantization. For these anchors, `EXIST_THRESHOLD_TOP = 0.8` is applied instead of the base threshold of `0.5`.

---

## Lateral deviation

The `LateralDeviation` value published to KUKSA is normalised:

- `0.0` → vehicle centred between the two lanes
- `+1.0` → vehicle drifting to the right
- `-1.0` → vehicle drifting to the left

The value is computed from the mean x of the `LATERAL_DEVIATION_N` lowest points (closest to the vehicle) of each lane, with a `CAMERA_OFFSET_NORM` correction applied to compensate for the physical camera offset.

---

## Data files

| File | Description |
|---|---|
| `/data/tusimple_res34_cut_v2.hef` | HEF compiled for Hailo-8 (backbone up to /pool/Conv) |
| `/data/tusimple_postprocess_weights.npz` | FC layer weights (W1, b1, W2, b2) |
| `/data/demo_lane_detection.mp4` | Annotated output video (with `--save`) |

---

## Team

| Member | Responsibility |
|---|---|
| David | Lane detection, perception pipeline, ADAS architecture |
| Vasco | Dataset labelling, YOLOv8 training, ADR |
| Ruben | Cruise control, logging, chassis, second camera |
