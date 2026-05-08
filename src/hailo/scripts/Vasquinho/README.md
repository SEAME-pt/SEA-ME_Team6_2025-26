# Vasquinho Scripts - Usage and Purpose

This folder contains wrappers and utilities used during Sprint 13 to compare ONNX and HEF behavior, and to reproduce Vasco-like postprocess settings.

## Scripts asked in review

### `pilot_ab_preprocess.sh`
- Current status: optional/experimental.
- Purpose: run an A/B pilot to compare preprocess/runtime variants (control A vs variant B) and print a compact metric table.
- Typical use: targeted debugging when confidence drift is suspected.

### `run_best_offline.py`
- Current status: used.
- Purpose: wrapper over `inference_video_sprint13.py` with Vasco-like defaults.
- Default knobs:
	- `--conf 0.30`
	- `--iou 0.40`
	- `--min-box-size 0.04`
	- `--global-nms-iou 0.25`
- Typical use: quick offline replay with historical Vasco thresholds/NMS behavior.

### `run_compare_all4_onnx_vs_hef.sh`
- Current status: used.
- Purpose: orchestrator for all 4 models (`yolov8s_detect`, `yolo26n_detect`, `yolov8n_seg`, `yolo26n_seg`) calling the per-model compare script.
- Typical use: run the same compare flow for a given `VIDEO_NAME` and produce one combined summary table.

### `run_compare_onnx_vs_hef_hostdecode.sh`
- Current status: used.
- Purpose: per-model compare flow:
	- ONNX hostdecode locally (Lenovo)
	- HEF hostdecode remotely (AGL)
	- copy outputs back and print side-by-side summary.
- Note: requires SSH/scp access to AGL.

### `run_onnx_hostdecode_sprint13.py`
- Current status: used.
- Purpose: run ONNXRuntime with the same host decode logic used in HEF flow (`decode_byom` parity path).
- Typical use: isolate pre-HEF behavior and compare against HEF output.

### `run_phase_f_comparacao_com_vasco.sh`
- Current status: used.
- Purpose: benchmark baseline Sprint13 postprocess vs Vasco-like postprocess on the same HEF and videos.
- Recommended output folder on AGL:
	- `/data/yolo_benchmark/results/fase_f_comparacao_com_vasco/`

## Dependency notes

- `run_compare_all4_onnx_vs_hef.sh` depends on `run_compare_onnx_vs_hef_hostdecode.sh`.
- `run_compare_onnx_vs_hef_hostdecode.sh` depends on `run_onnx_hostdecode_sprint13.py`.
- `run_phase_f_comparacao_com_vasco.sh` depends on `run_best_offline.py`.

## Quick Commands (copy/paste)

30-frame compare (table-focused):

```bash
VIDEO_NAME=teste1 MAX_FRAMES=30 CONF_DETECT=0.45 CONF_SEG=0.25 \
bash /home/seame/Documents/SEA-ME_Team6_2025-26/src/hailo/scripts/Vasquinho/run_compare_all4_onnx_vs_hef.sh
```

Full video compare (MAX_FRAMES=0):

```bash
VIDEO_NAME=teste1 MAX_FRAMES=0 CONF_DETECT=0.45 CONF_SEG=0.25 \
bash /home/seame/Documents/SEA-ME_Team6_2025-26/src/hailo/scripts/Vasquinho/run_compare_all4_onnx_vs_hef.sh
```

Organize `teste1` outputs into a timestamped consistency folder:

```bash
DEST=/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste1_$(date +%F_%H%M%S)
mkdir -p "$DEST"
cp -f /home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/teste1_* "$DEST"/
echo "$DEST"
```

Metric naming reminder:
- `p50_ms/p95_ms/p99_ms` are latency metrics in milliseconds.
- `p50_conf/p95_conf` are confidence percentiles (0..1), not latency.

## Keep or remove guidance

- Keep: `run_best_offline.py`, `run_compare_all4_onnx_vs_hef.sh`, `run_compare_onnx_vs_hef_hostdecode.sh`, `run_onnx_hostdecode_sprint13.py`, `run_phase_f_comparacao_com_vasco.sh`.
- Keep as optional debug tool: `pilot_ab_preprocess.sh`.
- Remove only after replacing their workflows in docs and CI/ops scripts.

## FAQ (Sprint 13)

### Why are boxes not green in ONNX/HEF compare videos?

- Compare flows use `inference_video_sprint13.py` (directly or through `run_onnx_hostdecode_sprint13.py`).
- That runtime draws boxes with class palette colors (`_PALETTE`), not fixed green.
- Green-only style exists in `run_best_vasco_original_style.py` (`BOX_COLOR = (0, 255, 0)`), which was a separate visual-style runner.

### Why were some videos only ~1.2s?

- Short compare clips were generated with `--max-frames 30` for fast parity checks.
- With 25 FPS source videos, 30 frames produce ~1.2 seconds.
- Full-length generation should run with `--max-frames 0` (or omit the option).

### Where are the source videos used for test1/test2?

- Lenovo source videos:
	- `/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset/teste1.mp4`
	- `/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset/teste2.mp4`
- AGL runtime copy (when executing on target):
	- `/data/yolo_benchmark/videos/teste1.mp4`
	- `/data/yolo_benchmark/videos/teste2.mp4`

### Where are organized compare outputs?

- Compare root:
	- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/`
- Test1 dedicated folder:
	- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste1_2026-05-08/`
- Test2 dedicated folder:
	- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/consistencia_teste2_2026-05-08/`
- Full-length (test1/test2) folder:
	- `/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/full_frames_test1_test2_2026-05-08/`