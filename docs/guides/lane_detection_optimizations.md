# Lane Detection Pipeline

## 1. Overview

The lane detection pipeline provides real-time lane keep assist (LKA) perception for the Sea:me PiRacer platform. It runs a lane-segmentation model split across the Hailo-8 NPU and the Raspberry Pi 5 CPU, computes lateral deviation from the detected ego-lanes, and publishes results to both the ADAS Manager (for control decisions) and the KUKSA VSS databroker (for HMI/telemetry).

Model: **UFLDv2 (Ultra-Fast Lane Detection v2)**, ResNet-34 backbone, TuSimple-pretrained with a project-specific fine-tune (1,359 annotated images from the SEAME track, 50 epochs, F1 ≈ 0.466).

## 2. Hardware & Software Context

| Component | Role |
|---|---|
| Raspberry Pi 5 | Main compute, runs AGL (Automotive Grade Linux / Yocto), executes FC head + post-processing |
| Hailo-8 (PCIe) | AI accelerator, executes the CNN backbone |
| IMX708 camera | Image source, 820×616, intrinsic calibration applied pre-inference |
| STM32 (ThreadX RTOS) | Receives lane/ADAS signals over CAN |

AGL constraints that shaped implementation choices: no `apt`/`pip` on the target (opkg only, requires an active Yocto build server), limited disk space, and a broken `pykms`/`drm_preview.py` import path in `picamera2` on AGL. Camera access is brokered through Diogo's `CameraBroker` service rather than direct `rpicam-vid`, since the IMX708 needs to be shared across inference processes.

## 3. Model Split Strategy

Hailo-8 does not efficiently handle fully-connected layers with large output channel counts (>2048). UFLDv2's classification head has two FC layers:

- **FC1**: 2048 × 2000
- **FC2**: 39,576 × 2048

To work around this, the ONNX graph is split at `/pool/Conv`:

- The **convolutional backbone** runs on the Hailo-8 NPU (~11 ms per frame).
- The **FC head** runs on the RPi5 CPU via NumPy/OpenBLAS (Cortex-A76 build).

This required an explicit **HWC → CHW transpose** of the Hailo output before feeding it into FC1, since Hailo returns convolutional output in HWC format while the original FC weights expect CHW.

### Model and dataset choice

TuSimple's 56 row anchors were chosen over CULane's 18 because the finer vertical sampling substantially improves curve detection accuracy — relevant given the tight-radius curves on the indoor SEAME track.

## 4. Optimizations Implemented

### 4.1 FC2 weight slicing (lossless, ~2× speedup)
The vehicle only needs to track two ego-lanes (immediate left and right lane boundaries), not the full multi-lane output UFLDv2 supports. FC2's weight matrix is sliced at model-load time to retain only the rows corresponding to ego-lanes 1 and 2.

- **Size reduction**: ~71% (FC2 weights reduced to ~88 MB)
- **Speed gain**: ~2× inference speedup on the FC head
- **Quality impact**: none — slicing removes only unused output rows, it does not approximate or degrade the retained lanes

### 4.2 C++ post-processing module
Post-processing (row anchor decoding, existence thresholding, coordinate reconstruction) was moved out of pure Python into a C++ shared library (`ufldv2_postprocess.so`), called from Python via a `ctypes` bridge class (`CppPostProcessor`). This removes Python-loop overhead from the hot path.

### 4.3 Camera calibration
Intrinsic calibration for the IMX708 (820×616) is applied to each frame before inference, correcting lens distortion so that lateral deviation measurements reflect real-world geometry rather than raw pixel-space artifacts.

### 4.4 Signal stabilization
- **Temporal smoothing** across frames to reduce jitter in lane position
- **Spatial moving average** across row anchors
- **Adaptive existence thresholds** for lane presence, rather than a fixed confidence cutoff
- **Lateral deviation calculation** derived from the smoothed ego-lane positions, published as the primary LKA signal

### 4.5 Dual-model inference (UFLDv2 + YOLOv8s)
The Hailo-8 is a single physical device — instantiating multiple `VDevice` objects (one per model) throws `HAILO_OUT_OF_PHYSICAL_DEVICES`. The correct pattern is a **single `VDevice` with two network groups**, one per model, sharing the NPU.

Because NPU inference is sequential, combined latency is additive:

| Configuration | Throughput |
|---|---|
| UFLDv2 solo | ~19–20 FPS |
| UFLDv2 + YOLOv8s combined | ~12.5 FPS |

### 4.6 Failed optimization: float16
Casting the FC head to float16 was attempted to reduce latency further. It caused value overflow in practice, degrading end-to-end latency to ~779 ms. This approach was abandoned in favor of float32.

### 4.7 Hailo DFC calibration set quality
An early calibration dataset used raw UINT8 images without ImageNet normalization for the Hailo Dataflow Compiler (DFC) quantization step. This measurably degraded output SNR (~26 dB → ~17 dB). The fix was to calibrate using float32, ImageNet-normalized images, matching the model's expected input distribution.

## 5. Data Flow & IPC

```
IMX708 camera
    → CameraBroker (shared camera access)
    → Hailo-8 backbone inference (~11 ms)
    → HWC→CHW transpose
    → FC1 / FC2 (sliced) on RPi5 CPU (NumPy/OpenBLAS)
    → C++ post-processing (ufldv2_postprocess.so)
    → temporal/spatial smoothing, adaptive thresholding
    → lateral deviation computation
    → Unix domain socket (/tmp/lane_keep_assist.sock)
        → ADAS Manager (C++17 FSM)
    → KUKSA databroker (gRPC v2, VSS)
        → Vehicle.ADAS.LaneKeepAssist.LateralDeviation (float)
        → Vehicle.ADAS.LaneKeepAssist.LaneStatus (string)
```

### ADAS Manager integration
The ADAS Manager is a C++17 finite state machine with modes: **Manual, Assisted, Autonomous, Emergency**.

- Manual → Autonomous must pass through Assisted (no direct jump).
- Emergency always returns to Manual; there is no automatic resume.
- IPC with the lane pipeline uses `SOCK_DGRAM` with binary struct packing (`struct.pack`).

Open items on this interface: CAN message IDs (`0x100`/`0x200` placeholders) still need to be aligned with the cruise-control/CAN integration owner; `lane_confidence` and `object_distance` fields in the shared `PerceptionData` struct are currently placeholders pending implementation.

### KUKSA / VSS integration
Uses the KUKSA VSS API v2 (`kuksa.val.v2` gRPC), not the legacy `KuksaClientThread`. Authentication is via JWT (RS256, `kuksa-vss: {"Vehicle.*": "r"}`), with `root_certificates` passed as a `Path` object and the protobuf float field named `float` (not `float_`). The KUKSA service runs under a dedicated `kuksa` user, so file permissions on certs/tokens matter.

## 6. Performance Summary

| Stage | Latency / Throughput |
|---|---|
| Hailo-8 backbone inference | ~11 ms/frame |
| UFLDv2 solo pipeline | ~19–20 FPS |
| UFLDv2 + YOLOv8s combined | ~12.5 FPS |
| FC2 slicing speedup | ~2× vs. unsliced FC head |
| FC2 weight size after slicing | ~88 MB (~71% reduction) |

## 7. Known Limitations & Open Work

- `lane_confidence` and `object_distance` in `PerceptionData` are not yet implemented (placeholders).
- CAN message IDs for lane signals are placeholders pending alignment with the CAN/cruise-control integration.
- Fine-tuned model quality (F1 ≈ 0.466 over 50 epochs) leaves room for improvement with more annotated data or additional training epochs.
- float16 precision is not viable for the FC head under the current implementation (see §4.6).

## 8. Glossary

- **UFLDv2** — Ultra-Fast Lane Detection v2, the row-anchor-based lane segmentation model used for perception.
- **HEF** — Hailo Executable Format, the compiled model format run on the Hailo-8 NPU.
- **VDevice** — Hailo virtual device handle; only one may exist per physical Hailo-8 device.
- **VSS** — Vehicle Signal Specification, the schema used by KUKSA for signal publishing.
- **AGL** — Automotive Grade Linux, the Yocto-based OS running on the Raspberry Pi 5.
