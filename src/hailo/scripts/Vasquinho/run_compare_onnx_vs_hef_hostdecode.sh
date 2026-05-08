#!/usr/bin/env bash
set -euo pipefail

# Compare ONNX pre-HEF vs HEF using the same host-side decode path.
# Local side: ONNXRuntime + decode_byom
# Remote AGL: HEF + inference_video_sprint13.py + decode_byom

REMOTE="root@10.21.220.191"
LOCAL_REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
LOCAL_PY="$LOCAL_REPO/.venv/bin/python"

MODEL_NAME="${MODEL_NAME:-yolov8s_detect}"
MODEL_TYPE="${MODEL_TYPE:-detect}"
VIDEO_NAME="${VIDEO_NAME:-teste1}"
CONF="${CONF:-0.45}"
IOU="${IOU:-0.55}"
MIN_BOX_SIZE="${MIN_BOX_SIZE:-0.0}"
GLOBAL_NMS_IOU="${GLOBAL_NMS_IOU:-0.0}"
MAX_FRAMES="${MAX_FRAMES:-0}"
HEF_VARIANT_SUFFIX="${HEF_VARIANT_SUFFIX:-}"

PHASE_E_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export"
LOCAL_VIDEO_DIR="/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset"
LOCAL_RESULTS="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode"

REMOTE_BASE="/data/yolo_benchmark"
REMOTE_SCRIPTS="$REMOTE_BASE/scripts"
REMOTE_RESULTS="$REMOTE_BASE/results/compare_onnx_vs_hef_hostdecode"

ONNX_PATH="$PHASE_E_DIR/${MODEL_NAME}_sprint13.onnx"
HEF_PATH="$PHASE_E_DIR/${MODEL_NAME}_sprint13${HEF_VARIANT_SUFFIX}.hef"
LOCAL_VIDEO="$LOCAL_VIDEO_DIR/${VIDEO_NAME}.mp4"

mkdir -p "$LOCAL_RESULTS"

if [[ ! -x "$LOCAL_PY" ]]; then
  echo "Missing local python environment: $LOCAL_PY" >&2
  exit 1
fi
if [[ ! -f "$ONNX_PATH" ]]; then
  echo "Missing ONNX: $ONNX_PATH" >&2
  exit 1
fi
if [[ ! -f "$HEF_PATH" ]]; then
  echo "Missing HEF: $HEF_PATH" >&2
  exit 1
fi
if [[ ! -f "$LOCAL_VIDEO" ]]; then
  echo "Missing local video: $LOCAL_VIDEO" >&2
  exit 1
fi

LOCAL_OUT="$LOCAL_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_onnx_hostdecode.mp4"

rm -f "$LOCAL_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_onnx_hostdecode.mp4" \
  "$LOCAL_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_onnx_hostdecode_stats.json" \
  "$LOCAL_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_hef_hostdecode.mp4" \
  "$LOCAL_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_hef_hostdecode_stats.json"

echo "[1/4] Run ONNX locally with host decode"
"$LOCAL_PY" "$LOCAL_REPO/src/hailo/scripts/Vasquinho/run_onnx_hostdecode_sprint13.py" \
  --onnx "$ONNX_PATH" \
  --source "$LOCAL_VIDEO" \
  --output "$LOCAL_OUT" \
  --model-type "$MODEL_TYPE" \
  --model-name "${MODEL_NAME}_onnx_hostdecode" \
  --conf "$CONF" --iou "$IOU" \
  --min-box-size "$MIN_BOX_SIZE" --global-nms-iou "$GLOBAL_NMS_IOU" \
  --sync-writer --label-decimals 3 \
  --max-frames "$MAX_FRAMES"

echo "[2/4] Copy HEF runtime to AGL"
echo "Using HEF variant suffix: '${HEF_VARIANT_SUFFIX}'"
scp "$LOCAL_REPO/src/hailo/scripts/inference_video_sprint13.py" "$REMOTE:$REMOTE_SCRIPTS/"
scp "$HEF_PATH" "$REMOTE:$REMOTE_BASE/models/hef/sprint13/"

echo "[3/4] Run HEF on AGL with same decode settings"
REMOTE_HEF_NAME="$(basename "$HEF_PATH")"
hef_run_ok=0
for attempt in 1 2 3; do
  if ssh "$REMOTE" \
    "REMOTE_RESULTS='$REMOTE_RESULTS' REMOTE_SCRIPTS='$REMOTE_SCRIPTS' REMOTE_HEF_NAME='$REMOTE_HEF_NAME' VIDEO_NAME='$VIDEO_NAME' MODEL_NAME='$MODEL_NAME' MODEL_TYPE='$MODEL_TYPE' CONF='$CONF' IOU='$IOU' MIN_BOX_SIZE='$MIN_BOX_SIZE' GLOBAL_NMS_IOU='$GLOBAL_NMS_IOU' MAX_FRAMES='$MAX_FRAMES' bash -s" <<'BASH'
set -euo pipefail
BASE=/data/yolo_benchmark
mkdir -p "$REMOTE_RESULTS"
python3 "$REMOTE_SCRIPTS/inference_video_sprint13.py" \
  --hef "$BASE/models/hef/sprint13/$REMOTE_HEF_NAME" \
  --source "$BASE/videos/${VIDEO_NAME}.mp4" \
  --output "$REMOTE_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_hef_hostdecode.mp4" \
  --model-type "$MODEL_TYPE" \
  --model-name "${MODEL_NAME}_hef_hostdecode" \
  --conf "$CONF" --iou "$IOU" \
  --min-box-size "$MIN_BOX_SIZE" --global-nms-iou "$GLOBAL_NMS_IOU" \
  --sync-writer --label-decimals 3 \
  $( [[ "$MAX_FRAMES" != "0" ]] && printf '%s' "--max-frames $MAX_FRAMES" )
BASH
  then
    hef_run_ok=1
    break
  fi
  if [[ "$attempt" -lt 3 ]]; then
    echo "Remote HEF run failed (attempt $attempt/3). Waiting 10s before retry..." >&2
    sleep 10
  fi
done

if [[ "$hef_run_ok" != "1" ]]; then
  echo "HEF run failed after 3 attempts." >&2
  exit 1
fi

echo "[4/4] Copy HEF results back and print comparison"
scp "$REMOTE:$REMOTE_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_hef_hostdecode.mp4" "$LOCAL_RESULTS/"
scp "$REMOTE:$REMOTE_RESULTS/${VIDEO_NAME}_${MODEL_NAME}_hef_hostdecode_stats.json" "$LOCAL_RESULTS/"

VIDEO_NAME="$VIDEO_NAME" MODEL_NAME="$MODEL_NAME" "$LOCAL_PY" - <<'PY'
import json
import os
from pathlib import Path

base = Path('/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode')
video = os.environ['VIDEO_NAME']
model = os.environ['MODEL_NAME']
stats = sorted(base.glob(f'{video}_{model}_*_stats.json'))
rows = []
for p in stats:
    d = json.loads(p.read_text())
    backend = d.get('backend', 'hef_hostdecode')
    rows.append((
        d.get('model'),
        backend,
        d.get('real_fps'),
        d.get('pipeline_fps_stage_total'),
        d.get('mean_conf'),
        d.get('p50_conf'),
        d.get('p95_conf'),
    ))

print('\nmodel                      backend           real_fps  pipe_stage  mean_conf  p50_conf  p95_conf')
print('-----------------------------------------------------------------------------------------------')
for model, backend, real_fps, pipe_stage, mean_conf, p50_conf, p95_conf in rows:
    def f(v, n):
        return 'NA' if v is None else f'{v:.{n}f}'
    print(f'{str(model):<26} {str(backend):<16} {f(real_fps,2):>8} {f(pipe_stage,2):>11} {f(mean_conf,4):>10} {f(p50_conf,4):>9} {f(p95_conf,4):>9}')

print(f'\nResults folder: {base}')
PY