# Hailo YOLO Master Context

This file is the single operational context entry point for Hailo/YOLO work in SEA:ME Team 6.
It reflects the current state as of `2026-04-25`.

## Table of contents

1. [Scope and purpose](#1-scope-and-purpose)
2. [Current technical decisions](#2-current-technical-decisions)
3. [Model and artifact registry](#3-model-and-artifact-registry)
4. [Execution environments and canonical paths](#4-execution-environments-and-canonical-paths)
5. [Post-processing status](#5-post-processing-status)
6. [What is completed](#6-what-is-completed)
7. [Current benchmark closure target](#7-current-benchmark-closure-target)
8. [Dataset and training location policy](#8-dataset-and-training-location-policy)
9. [Known risks and blockers](#9-known-risks-and-blockers)
10. [Immediate runbook (5-HEF final batch)](#10-immediate-runbook-5-hef-final-batch)
11. [Open questions](#11-open-questions)
12. [Remote Lenovo-AGL flow](#12-remote-lenovo-agl-flow)
13. [SSH key setup used](#13-ssh-key-setup-used)

## 1) Scope and purpose

This context covers:

- YOLO training and comparison on Lenovo,
- ONNX and HEF conversion lineage,
- deployment and runtime benchmarking on AGL (Hailo-8),
- final comparison closure for the active 5-HEF set.

## 2) Current technical decisions

- File naming is informative, but not authoritative for runtime tensor names.
- `hailortcli parse-hef` is the source of truth for HEF architecture and stream metadata.
- Canonical AGL benchmark root is `/data/yolo_benchmark`.
- Legacy aliases `/data/scripts`, `/data/models`, `/data/results` were removed intentionally.
- AGL is inference-only in the current workflow.
- Training datasets stay on Lenovo and are not copied to AGL.
- Post-processing behavior is part of the benchmark interpretation and must be tracked per model.
- For fair model comparison, all candidates must run with the same duration, same input source, and same output collection pattern.

## 3) Model and artifact registry

### 3.1 Active final comparison set

| HEF | Task type | Status |
|---|---|---|
| `yolo26n_seg_320_h8_no_nms.hef` | segmentation | available on AGL |
| `yolov8n_seg_h8.hef` | segmentation | available on AGL |
| `yolov8n_h8.hef` | detection | available on AGL |
| `yolov8s_baseline_h8.hef` | detection | available on AGL |
| `yolov8s_vasco_trained_h8.hef` | detection | available on AGL |

### 3.2 Related local scripts

- `src/hailo/scripts/train_compare_yolov8s_vs_vasco.sh`
	- trains local `yolov8s` and compares `.pt` validation vs Vasco model,
	- supports `SKIP_TRAIN=1`.
- `src/hailo/scripts/compare_yolov8s_hef_runtime.sh`
	- generic pairwise HEF runtime comparison runner,
	- configurable labels, scripts, remote HEF paths, and image sources.
- `src/hailo/scripts/inference_camera_scalercrop_yolov8n.py`
	- dedicated YOLOv8n detection wrapper using the shared YOLOv8 detection runtime path.

### 3.3 Conversion lineage notes

- `yolov8s.pt -> yolov8s.onnx` export completed.
- Baseline compile succeeded: `yolov8s_baseline_h8.hef`.
- Custom YOLOv8s ONNX compile attempts failed due to parser mismatch; baseline path remains the stable reference.

## 4) Execution environments and canonical paths

### 4.1 Repository root

- `/home/seame/Documents/SEA-ME_Team6_2025-26`

### 4.2 Lenovo (training, conversion, archive)

- benchmark workspace: `/home/seame/Documents/AI/Yolo_benchmark`
- hailo shared workspace: `/home/seame/Documents/AI/hailo/shared_with_docker`
- local benchmark output archive: `/home/seame/Documents/AI/Yolo_benchmark/hef_compare_generic`

### 4.3 AGL (runtime target)

- target: `root@10.21.220.191`
- canonical root: `/data/yolo_benchmark`
- models: `/data/yolo_benchmark/models/hef`
- scripts: `/data/yolo_benchmark/scripts`
- results: `/data/yolo_benchmark/results`

## 5) Post-processing status

The project currently uses both device-side and host-side post-processing paths.

### 5.1 Device-side NMS

- `yolov8s` uses a device-side NMS output path when the HEF exposes `yolov8s/yolov8_nms_postprocess`.
- The local runtime implementation is in `src/hailo/scripts/inference_camera_scalercrop_yolov8s.py`.
- This is the stable reference path for the baseline YOLOv8s benchmark.

### 5.2 Host-side post-processing

- `yolo26n_seg_320_h8_no_nms.hef` is a no-device-NMS path and depends on host-side post-processing.
- Its current local implementation is in `src/hailo/scripts/inference_camera_scalercrop_yolo26n_seg.py`.
- `yolov8n_seg_h8.hef` also uses dedicated segmentation post-processing logic in `src/hailo/scripts/inference_camera_scalercrop_yolov8n_seg.py`.

### 5.3 Custom YOLOv8s post-process attempt

- A custom `.alls` path was tried for YOLOv8s post-processing in `/home/seame/Documents/AI/hailo/shared_with_docker/scripts/yolov8s_bbox_decoding_only_fixed.alls`.
- That attempt explicitly configured `nms_postprocess(...)` for a bbox-decoding-only path.
- The intent was to control the post-processing path during custom compilation.
- Outcome: the custom ONNX -> HEF route did not close successfully because of parser/model mismatch, so it is investigation history, not the recommended deployment path.

### 5.4 Benchmark reading rule

- Final benchmark interpretation must distinguish device-NMS paths from host-NMS or host-heavy post-processing paths.
- This matters because E2E performance includes post-processing cost, not only inference cost.

### 5.5 Quick mapping table

| Model / HEF | Post-processing placement | Main implementation | Benchmark reading |
|---|---|---|---|
| `yolov8s_baseline_h8.hef` | mostly device-side NMS | `src/hailo/scripts/inference_camera_scalercrop_yolov8s.py` | best reference for stable device-side detection path |
| `yolov8s_vasco_trained_h8.hef` | expected same runtime family as YOLOv8s detection path | `src/hailo/scripts/inference_camera_scalercrop_yolov8s.py` or equivalent aligned script on AGL | compare carefully against baseline because model weights change, but runtime contract should stay close |
| `yolov8n_h8.hef` | detection path with device-side NMS contract | `src/hailo/scripts/inference_camera_scalercrop_yolov8n.py` | compare as lighter detection candidate against the two YOLOv8s variants |
| `yolov8n_seg_h8.hef` | host-side segmentation decode/post-process | `src/hailo/scripts/inference_camera_scalercrop_yolov8n_seg.py` | segmentation result includes non-trivial CPU-side postprocess cost |
| `yolo26n_seg_320_h8_no_nms.hef` | host-side post-process, explicitly no device NMS | `src/hailo/scripts/inference_camera_scalercrop_yolo26n_seg.py` | treat as experimental-host-NMS path when reading final E2E results |

## 6) What is completed

- Extracted and inspected Vasco training artifacts/parameters.
- Trained local YOLOv8s and compared against Vasco `.pt` on common validation setup.
- Added robust summary generation for `.pt` comparison.
- Exported ONNX and produced `yolov8s_baseline_h8.hef`.
- Tried custom YOLOv8s post-processing integration during compile through `.alls` model-script work.
- Copied all relevant HEFs to AGL canonical model folder.
- Reorganized AGL paths under `/data/yolo_benchmark`.
- Removed old top-level alias links in `/data` (`scripts`, `models`, `results`).

## 7) Current benchmark closure target

Goal: close the final runtime benchmark with all 5 HEFs listed in Section 3.1.

Fairness criteria:

- same AGL board,
- same script family and runtime options,
- same input source (`--images-dir` or equivalent consistent stream),
- same benchmark duration,
- same output artifacts (`.mp4`, `_stats.csv`, summary table).

Minimum deliverable:

- one consolidated table with FPS/latency per HEF,
- video evidence for qualitative comparison,
- short winner statement per task group:
	- detection group,
	- segmentation group.

## 8) Dataset and training location policy

- Training datasets remain on Lenovo.
- AGL should not host Roboflow/training datasets in this phase.
- AGL stores only what is required for runtime benchmarking:
	- HEFs,
	- runtime scripts,
	- benchmark outputs.

## 9) Known risks and blockers

- AGL disk space has been critically low during this phase and can break benchmark output writing.
- Some historical commands/scripts still reference legacy `/data/scripts` or `/data/results` paths and must be adjusted to `/data/yolo_benchmark/...`.
- Custom YOLOv8s ONNX compile path is still unresolved; use validated baseline HEF for final closure.
- Final benchmark conclusions must account for different post-processing placement across models.

## 10) Immediate runbook (5-HEF final batch)

1. Verify free space on AGL before execution.
2. Clean only obsolete large outputs if space is insufficient.
3. Run each HEF with identical benchmark parameters.
4. Store each run under `/data/yolo_benchmark/results/final_5hef/<model_label>/`.
5. Copy results back to Lenovo archive.
6. Generate final comparison summary.

Reference command style (adapt paths/script per model type):

```bash
ssh root@10.21.220.191 "python3 /data/yolo_benchmark/scripts/<inference_script>.py 60 --images-dir <shared_input_dir> --loop --save --output /data/yolo_benchmark/results/final_5hef/<label>/<label>.mp4"
```

Expected artifact set per model:

- `<label>.mp4`
- `<label>_stats.csv`

## 11) Open questions

- Should we keep one unified inference entrypoint script for all 5 HEFs, or continue model-specific scripts with common CLI contract?
- After final runtime closure, do we open a separate GT scoring track (mAP/mIoU) on Lenovo only?

## 12) Remote Lenovo-AGL flow

Validated execution pattern when AGL storage is limited:

1. Keep full datasets on Lenovo under `offline_round2_frames`.
2. Stage only a small batch of frames to AGL temporary folder in `/tmp`.
3. Run inference on AGL/Hailo.
4. Copy `_stats.csv` and `.log` back to a single run folder on Lenovo.
5. Remove temporary artifacts from AGL `/tmp`.

Important CLI note:

- Current inference scripts use positional `duration` argument.
- Use `... <script>.py --images-dir <dir> --output <out>.mp4 15`.
- Do not use `--duration` with these scripts.

Latest consolidated 5-model run folder (Lenovo):

- `/home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/remote_agl_5models_20260425_015624`

This folder contains 10 files:

- 5 logs (`*.log`)
- 5 stats files (`*_stats.csv`)

## 13) SSH key setup used

The following setup was applied to enable non-interactive automation.

Lenovo -> AGL (for command execution/scp):

```bash
ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519 -N '' -C 'seame6-to-agl'
ssh-copy-id -i ~/.ssh/id_ed25519.pub root@10.21.220.191
ssh -o BatchMode=yes root@10.21.220.191 "echo AGL_KEY_OK"
```

AGL -> Lenovo (for reverse ssh/scp when needed):

```bash
ssh root@10.21.220.191 "mkdir -p /root/.ssh && chmod 700 /root/.ssh && [ -f /root/.ssh/id_ed25519 ] || ssh-keygen -t ed25519 -f /root/.ssh/id_ed25519 -N '' -C 'agl-to-lenovo'; cat /root/.ssh/id_ed25519.pub"
```

Append emitted public key to Lenovo:

```bash
mkdir -p ~/.ssh && chmod 700 ~/.ssh
touch ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys
# add the AGL public key line ending with comment 'agl-to-lenovo'
```

Validation:

```bash
ssh -o BatchMode=yes root@10.21.220.191 "ssh -o BatchMode=yes -o StrictHostKeyChecking=no seame@10.21.100.6 'echo LENOVO_KEY_OK'"
```
