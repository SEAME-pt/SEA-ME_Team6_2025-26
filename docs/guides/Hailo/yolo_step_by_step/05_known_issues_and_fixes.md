# 05 - Known Issues and Fixes

## Table of contents

1. [`ModuleNotFoundError: hailo_demo_common`](#1-modulenotfounderror-hailo_demo_common)
2. [`--duration` not recognized in the fixed script](#2--duration-not-recognized-in-the-fixed-script)
3. [Streams `yolo26n_seg/*` not found](#3-streams-yolo26n_seg-not-found)
4. [Ghost boxes in the video](#4-ghost-boxes-in-the-video)
5. [Mandatory pre-benchmark checklist](#5-mandatory-pre-benchmark-checklist)

## 1) `ModuleNotFoundError: hailo_demo_common`

Symptom:
- script in `/data/scripts` fails to import `hailo_demo_common`.

Cause:
- the module exists in `/data/scripts/yolo_realtime/hailo_demo_common.py`, but not in the same directory as the script being executed.

Fix:

```bash
ssh root@10.21.220.191 "cp -f /data/scripts/yolo_realtime/hailo_demo_common.py /data/scripts/"
```

## 2) `--duration` not recognized in the fixed script

Symptom:
- `error: unrecognized arguments: --duration`

Cause:
- the script parser uses a positional `duration` argument.

Fix:

```bash
python3 inference_camera_scalercrop_yolo26n_seg_FIXED.py 30 --save --output /data/results/bench_runs/yolo26n_seg/round3/test_FIXED.mp4
```

## 3) Streams `yolo26n_seg/*` not found

Symptom:
- warning about input stream not found and inconsistent results.

Cause:
- the current `yolo26n-seg` HEF exposes internal streams with the `yolov8n_seg/*` prefix.

Fix:
- validate names with `hailortcli parse-hef`;
- align the script with the real HEF streams.

## 4) Ghost boxes in the video

Symptom:
- overlays with excessive or incoherent detections.

Likely cause:
- wrong output parser for the real HEF layout,
- or incorrect stream names,
- or poorly tuned thresholds.

Fix:
- use the fixed script,
- confirm the streams with `parse-hef`,
- tune `CONF_THRESH` and `IOU_THRESH` only if necessary.

## 5) Mandatory pre-benchmark checklist

1. `hailortcli parse-hef` executed for each candidate HEF.
2. The script uses exactly the streams reported by the parse.
3. Logs and videos saved using round naming (`round3`, `round4`, ...).
4. Results copied to Lenovo for peer review.
