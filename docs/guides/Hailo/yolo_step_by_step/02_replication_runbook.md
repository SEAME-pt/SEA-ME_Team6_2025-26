# 02 - Replication Runbook (Execution Layer)

This runbook is the fast execution layer.
The full rationale by phase is in `00_full_lifecycle_e2e_step_by_step.md`.

## Table of contents

1. [Objective](#0-objective)
2. [Prerequisites](#1-prerequisites)
3. [Validate artifacts on the AGL](#2-validate-artifacts-on-the-agl)
4. [Prepare the fixed script on the AGL](#3-prepare-the-fixed-script-on-the-agl)
5. [Quick yolo26n-seg test](#4-quick-yolo26n-seg-test)
6. [Full 3-model benchmark](#5-full-3-model-benchmark)
7. [Copy results back to Lenovo](#6-copy-results-back-to-lenovo)
8. [Closeout checklist](#7-closeout-checklist)

## 0) Objective

Execute the current state in a controlled way:
- inference with `yolov8s`, `yolov8n-seg`, and `yolo26n-seg`;
- stream validation via `parse-hef`;
- quick test and full round execution;
- collect videos, logs, and stats for comparison.

## 0.1) When to use this file

- use this file to execute the already-defined steps quickly;
- use `00_full_lifecycle_e2e_step_by_step.md` when you need the full technical context from start to finish.

## 1) Prerequisites

- Lenovo with SSH access to the AGL (`root@10.21.220.191`)
- scripts on Lenovo in `/home/seame/Documents/AI/Yolo_benchmark/scripts`
- HEFs on the AGL in `/data`

## 2) Validate artifacts on the AGL

```bash
ssh root@10.21.220.191 "ls -lh /data/*.hef"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8s.hef | sed -n '1,25p'"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolov8n_seg_h8.hef | sed -n '1,35p'"
ssh root@10.21.220.191 "hailortcli parse-hef /data/yolo26n_seg_320_h8_no_nms.hef | sed -n '1,35p'"
```

## 3) Prepare the fixed script on the AGL

```bash
scp /home/seame/Documents/AI/Yolo_benchmark/scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py root@10.21.220.191:/data/scripts/
ssh root@10.21.220.191 "cp -f /data/scripts/yolo_realtime/hailo_demo_common.py /data/scripts/"
```

## 4) Quick yolo26n-seg test

```bash
ssh root@10.21.220.191 "mkdir -p /data/results/bench_runs/yolo26n_seg/round3"
ssh root@10.21.220.191 "python3 /data/scripts/inference_camera_scalercrop_yolo26n_seg_FIXED.py 30 --save --output /data/results/bench_runs/yolo26n_seg/round3/test_FIXED.mp4"
```

## 5) Full 3-model benchmark

```bash
scp /home/seame/Documents/AI/Yolo_benchmark/scripts/run_benchmark_round3_offline.sh root@10.21.220.191:/data/scripts/
ssh root@10.21.220.191 "chmod +x /data/scripts/run_benchmark_round3_offline.sh && /data/scripts/run_benchmark_round3_offline.sh"
```

## 6) Copy results back to Lenovo

```bash
mkdir -p /home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/yolo26n_seg/round3
scp root@10.21.220.191:/data/results/bench_runs/yolo26n_seg/round3/* /home/seame/Documents/AI/Yolo_benchmark/results/bench_runs/yolo26n_seg/round3/
```

## 7) Closeout checklist

1. Video generated and reproducible on Lenovo.
2. Execution log saved.
3. `stats.csv` present.
4. Comparison table updated.
5. Issue opened if there is a mismatch between `parse-hef` and the script.
