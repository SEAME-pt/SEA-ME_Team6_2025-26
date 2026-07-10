# 01 - Quickstart From Zero

This guide is for anyone arriving today and needing to reproduce the inference and benchmark flow with no prior context.

## Table of contents

1. [Main paths and environment](#1-main-paths-and-environment)
2. [Active scripts](#2-active-scripts)
3. [HEF sanity check on the AGL](#3-hef-sanity-check-on-the-agl)
4. [Quick yolo26n-seg fix test](#4-quick-yolo26n-seg-fix-test)
5. [Copy results back to Lenovo](#5-copy-results-back-to-lenovo)
6. [What to validate in the video](#6-what-to-validate-in-the-video)

## 1) Main paths and environment

- Repo: `/home/seame/Documents/SEA-ME_Team6_2025-26`
- Lenovo benchmark workspace: `/home/seame/Documents/AI/Yolo_benchmark`
- AGL target: `root@10.21.220.191`
- HEFs on AGL: `/data/*.hef`
- Results on AGL: `/data/results/bench_runs`

## 2) Active scripts

In the repo:
- `src/hailo/scripts/inference_camera_scalercrop_yolov8s.py`
- `src/hailo/scripts/inference_camera_scalercrop_yolov8n_seg.py`
- `src/hailo/scripts/inference_camera_scalercrop_yolo26n_seg.py`

In Lenovo workspace (recent working versions):
- `/home/seame/Documents/AI/Yolo_benchmark/scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py`
- `/home/seame/Documents/AI/Yolo_benchmark/scripts/run_benchmark_round3_offline.sh`
- `/home/seame/Documents/AI/Yolo_benchmark/scripts/setup_test_rounds.sh`
- `/home/seame/Documents/AI/Yolo_benchmark/scripts/prepare_offline_tests.sh`

## 3) HEF sanity check on the AGL

Run from Lenovo:

```bash
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8s.hef | sed -n '1,25p'"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8n_seg_h8.hef | sed -n '1,35p'"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolo26n_seg_320_h8_no_nms.hef | sed -n '1,35p'"
```

Goal:
- confirm that the HEF opens,
- confirm the real input and output streams,
- confirm whether it is a baseline or host-NMS artifact.

## 4) Quick yolo26n-seg fix test

```bash
scp /home/seame/Documents/AI/Yolo_benchmark/scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py root@10.21.220.191:/data/scripts/
ssh root@10.21.220.191 "cp -f /data/scripts/yolo_realtime/hailo_demo_common.py /data/scripts/"
ssh root@10.21.220.191 "python3 /data/scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py 30 --save --output /data/results/bench_runs/yolo26n_seg/round3/test_FIXED_quick.mp4"
```

Note:
- the script uses a positional `duration` argument (`30`), not `--duration`.

## 5) Copy results back to Lenovo

```bash
mkdir -p /home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/yolo26n_seg/round3
scp root@10.21.220.191:/data/results/bench_runs/yolo26n_seg/round3/* /home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/yolo26n_seg/round3/
```

## 6) What to validate in the video

- whether there are ghost boxes (failure),
- whether detections per frame are stable (for example 3-4 objects in the current scenario),
- whether the latency/FPS overlay is coherent.

If it fails, follow `05_known_issues_and_fixes.md`.
