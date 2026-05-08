#!/usr/bin/env bash
set -euo pipefail

LOCAL_REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
LOCAL_PY="$LOCAL_REPO/.venv/bin/python"
PHASE_E_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export"

VIDEO_NAME="${VIDEO_NAME:-teste1}"
MAX_FRAMES="${MAX_FRAMES:-30}"
CONF="${CONF:-0.45}"
CONF_DETECT="${CONF_DETECT:-$CONF}"
CONF_SEG="${CONF_SEG:-0.25}"
IOU="${IOU:-0.55}"
HEF_VARIANT_SUFFIX="${HEF_VARIANT_SUFFIX:-}"

MODELS=(
  "yolov8s_detect detect"
  "yolo26n_detect detect"
  "yolov8n_seg seg"
  "yolo26n_seg seg"
)

for row in "${MODELS[@]}"; do
  read -r model mtype <<<"$row"
  model_conf="$CONF_DETECT"
  if [[ "$mtype" == "seg" ]]; then
    model_conf="$CONF_SEG"
  fi

  hef_path="$PHASE_E_DIR/${model}_sprint13${HEF_VARIANT_SUFFIX}.hef"
  if [[ ! -f "$hef_path" ]]; then
    echo
    echo "=== Skip $model (missing HEF variant: $hef_path) ==="
    continue
  fi

  echo
  echo "=== Compare $model ($mtype) on $VIDEO_NAME ==="
  MODEL_NAME="$model" \
  MODEL_TYPE="$mtype" \
  VIDEO_NAME="$VIDEO_NAME" \
  MAX_FRAMES="$MAX_FRAMES" \
  CONF="$model_conf" \
  IOU="$IOU" \
  HEF_VARIANT_SUFFIX="$HEF_VARIANT_SUFFIX" \
    bash "$LOCAL_REPO/src/hailo/scripts/Vasquinho/run_compare_onnx_vs_hef_hostdecode.sh"
done

VIDEO_NAME="$VIDEO_NAME" "$LOCAL_PY" - <<'PY'
import json
import os
from pathlib import Path

video = os.environ['VIDEO_NAME']
base = Path('/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode')
files = sorted(base.glob(f'{video}_*_hostdecode_stats.json'))
rows = []
for p in files:
    d = json.loads(p.read_text())
    rows.append((
        d.get('model'),
        d.get('model_type'),
        d.get('backend', 'hef_hostdecode'),
        d.get('num_frames'),
        d.get('real_fps'),
        d.get('mean_conf'),
        d.get('p50_conf'),
        d.get('p95_conf'),
    ))

print('\nmodel                           type    backend           frames  real_fps  mean_conf  p50_conf  p95_conf')
print('----------------------------------------------------------------------------------------------------------')
for model, mtype, backend, frames, real_fps, mean_conf, p50_conf, p95_conf in rows:
    def f(v, n):
        return 'NA' if v is None else f'{v:.{n}f}'
    print(f'{str(model):<31} {str(mtype):<7} {str(backend):<16} {str(frames):>6} {f(real_fps,2):>9} {f(mean_conf,4):>10} {f(p50_conf,4):>9} {f(p95_conf,4):>9}')
PY