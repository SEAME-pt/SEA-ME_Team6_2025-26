#!/usr/bin/env bash
set -euo pipefail

# Pilot A/B preprocess parity test:
# - A: reuse known-good Sprint13 HEF (control)
# - B: optional compile of a variant HEF (no extra model-script normalization)
# - run both on AGL with same runtime settings
# - copy outputs back to Lenovo and print comparison table

REMOTE="root@10.21.220.191"
LOCAL_REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
DOCKER_IMAGE="${DOCKER_IMAGE:-hailo8_ai_sw_suite_2025-10:1}"

PHASE_E_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export"
ONNX_PATH="$PHASE_E_DIR/yolov8s_detect_sprint13.onnx"
ALLS_B_TMP="/tmp/yolov8s_detect_no_nms_no_norm.alls"
CALIB_PATH="${CALIB_PATH:-}"
COMPILE_B="${COMPILE_B:-1}"

HEF_A_LOCAL="$PHASE_E_DIR/yolov8s_detect_sprint13.hef"
HEF_B_LOCAL="${HEF_B_LOCAL:-$PHASE_E_DIR/yolov8s_detect_sprint13_pilot_B_no_norm.hef}"

REMOTE_BASE="/data/yolo_benchmark"
REMOTE_SCRIPTS="$REMOTE_BASE/scripts"
REMOTE_HEF_DIR="$REMOTE_BASE/models/hef/sprint13"
REMOTE_RESULTS="$REMOTE_BASE/results/fase_f_preprocess_ab"

LOCAL_DEST="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/fase_f_preprocess_ab"

if [[ ! -f "$HEF_A_LOCAL" ]]; then
  echo "Missing baseline HEF A (control): $HEF_A_LOCAL" >&2
  exit 1
fi
if [[ "$COMPILE_B" == "1" ]]; then
  if [[ ! -f "$ONNX_PATH" ]]; then
    echo "Missing ONNX: $ONNX_PATH" >&2
    exit 1
  fi
  if [[ -z "$CALIB_PATH" ]]; then
    # Try known calibration folders in priority order.
    CANDIDATE_CALIBS=(
      "/home/seame/Documents/AI/hailo/shared_with_docker/calibration_images_17c"
      "/home/seame/Documents/AI/hailo/shared_with_docker/calibration_images"
      "/home/seame/Documents/AI/hailo/calibration_images"
    )
    for c in "${CANDIDATE_CALIBS[@]}"; do
      if [[ -d "$c" ]]; then
        CALIB_PATH="$c"
        break
      fi
    done
  fi
  if [[ -z "$CALIB_PATH" || ! -d "$CALIB_PATH" ]]; then
    echo "Missing calibration folder." >&2
    echo "Set CALIB_PATH explicitly, for example:" >&2
    echo "  CALIB_PATH=/home/seame/Documents/AI/hailo/shared_with_docker/calibration_images bash $0" >&2
    exit 1
  fi
  echo "Using calibration folder: $CALIB_PATH"
else
  echo "COMPILE_B=0, skipping ONNX/calibration checks"
fi

cat > "$ALLS_B_TMP" <<'EOF'
# pilot B: keep host-side postprocess (no nms command) and remove extra normalization.
# This avoids the default yolov8s.alls NMS parser path that expects conv heads.
EOF

echo "[1/5] Use existing Sprint13 HEF as control A"
echo "A control HEF: $HEF_A_LOCAL"

if [[ "$COMPILE_B" == "1" ]]; then
  echo "[2/5] Compile pilot HEF B (no-nms model-script, no normalization command)"
  docker run --rm -u 0:0 \
    -v "$PHASE_E_DIR:/phase_e" \
    -v "$CALIB_PATH:/calib" \
    -v "$ALLS_B_TMP:/work/b.alls" \
    "$DOCKER_IMAGE" \
    bash -lc '
      set -euo pipefail
      cd /
      hailomz compile yolov8s --ckpt /phase_e/yolov8s_detect_sprint13.onnx --hw-arch hailo8 --calib-path /calib --model-script /work/b.alls
      cp yolov8s.hef /phase_e/yolov8s_detect_sprint13_pilot_B_no_norm.hef
    ' || {
      echo "B compile failed. Common causes:" >&2
      echo "  - allocator cannot find valid partition for this variant" >&2
      echo "  - low-calibration + CPU-only quantization instability" >&2
      echo "Suggestion: keep COMPILE_B=0 and provide an already compiled B HEF path." >&2
      exit 1
    }
else
  echo "[2/5] Skip compile B (COMPILE_B=0)"
  if [[ ! -f "$HEF_B_LOCAL" ]]; then
    echo "Missing B HEF while COMPILE_B=0: $HEF_B_LOCAL" >&2
    echo "Either set COMPILE_B=1 or provide this file with HEF_B_LOCAL=/path/to/file.hef." >&2
    exit 1
  fi
fi

echo "[3/5] Copy scripts + HEFs to AGL"
scp "$LOCAL_REPO/src/hailo/scripts/inference_video_sprint13.py" "$REMOTE:$REMOTE_SCRIPTS/"
scp "$HEF_A_LOCAL" "$REMOTE:$REMOTE_HEF_DIR/"
scp "$HEF_B_LOCAL" "$REMOTE:$REMOTE_HEF_DIR/"

echo "[4/5] Run A/B on AGL"
ssh "$REMOTE" "bash -s" <<'BASH'
set -euo pipefail
BASE=/data/yolo_benchmark
SCRIPT=$BASE/scripts/inference_video_sprint13.py
RESULTS=$BASE/results/fase_f_preprocess_ab
mkdir -p "$RESULTS"

python3 "$SCRIPT" \
  --hef "$BASE/models/hef/sprint13/yolov8s_detect_sprint13.hef" \
  --source "$BASE/videos/teste1.mp4" \
  --output "$RESULTS/teste1_yolov8s_pilot_A_control.mp4" \
  --model-type detect --model-name yolov8s_pilot_A_control \
  --conf 0.45 --iou 0.55 --sync-writer --label-decimals 3

python3 "$SCRIPT" \
  --hef "$BASE/models/hef/sprint13/$(basename "$HEF_B_LOCAL")" \
  --source "$BASE/videos/teste1.mp4" \
  --output "$RESULTS/teste1_yolov8s_pilot_B_no_norm.mp4" \
  --model-type detect --model-name yolov8s_pilot_B_no_norm \
  --conf 0.45 --iou 0.55 --sync-writer --label-decimals 3
BASH

echo "[5/5] Copy A/B outputs back to Lenovo and print table"
mkdir -p "$LOCAL_DEST"
scp "$REMOTE:$REMOTE_RESULTS/*.mp4" "$LOCAL_DEST/"
scp "$REMOTE:$REMOTE_RESULTS/*_stats.json" "$LOCAL_DEST/"

python3 - <<'PY'
import glob, json, os
base = '/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/fase_f_preprocess_ab'
files = sorted(glob.glob(base + '/*_stats.json'))
rows = []
for p in files:
    d = json.load(open(p))
    bn = os.path.basename(p)
    if 'pilot_A_control' in bn:
      mode = 'A_control'
    elif 'pilot_B_no_norm' in bn:
        mode = 'B_no_norm'
    else:
        mode = 'unknown'
    rows.append((mode, d.get('real_fps'), d.get('pipeline_fps_stage_total'), d.get('mean_conf'), d.get('p50_conf'), d.get('p95_conf')))

print('\nmode       real_fps  pipe_stage  mean_conf  p50_conf  p95_conf')
print('--------------------------------------------------------------')
for mode, rf, pf, mc, p50, p95 in sorted(rows):
    def f(x,n):
        return 'NA' if x is None else f'{x:.{n}f}'
    print(f'{mode:<10} {f(rf,2):>8} {f(pf,2):>11} {f(mc,4):>10} {f(p50,4):>9} {f(p95,4):>9}')
print(f'\nTotal rows: {len(rows)}')
PY

echo "Done. Local outputs: $LOCAL_DEST"
