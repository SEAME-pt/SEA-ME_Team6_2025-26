#!/usr/bin/env bash
set -euo pipefail

# Run from Lenovo host. It copies scripts to AGL, executes baseline vs vasco-like
# runs on the same HEF/videos, and stores outputs under:
#   /data/yolo_benchmark/results/fase_f_comparacao_com_vasco

REMOTE="root@10.21.220.191"
LOCAL_REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
REMOTE_BASE="/data/yolo_benchmark"
REMOTE_SCRIPTS="$REMOTE_BASE/scripts"
REMOTE_RESULTS="$REMOTE_BASE/results/fase_f_comparacao_com_vasco"

scp "$LOCAL_REPO/src/hailo/scripts/inference_video_sprint13.py" "$REMOTE:$REMOTE_SCRIPTS/"
scp "$LOCAL_REPO/src/hailo/scripts/Vasquinho/run_best_offline.py" "$REMOTE:$REMOTE_SCRIPTS/"

ssh "$REMOTE" "bash -s" <<'BASH'
set -euo pipefail

BASE=/data/yolo_benchmark
RESULTS=$BASE/results/fase_f_comparacao_com_vasco
SCRIPT_BASE=$BASE/scripts/inference_video_sprint13.py
SCRIPT_VASCO=$BASE/scripts/run_best_offline.py
HEF=$BASE/models/hef/sprint13/yolov8s_detect_sprint13.hef
VIDEOS=(teste1.mp4 teste2.mp4)
mkdir -p "$RESULTS"

for v in "${VIDEOS[@]}"; do
  vname="${v%.mp4}"

  # Baseline (current Sprint13 behavior)
  python3 "$SCRIPT_BASE" \
    --hef "$HEF" \
    --source "$BASE/videos/$v" \
    --output "$RESULTS/${vname}_yolov8s_detect_baseline.mp4" \
    --model-type detect \
    --model-name yolov8s_detect_baseline \
    --conf 0.45 --iou 0.55 \
    --sync-writer --label-decimals 3

    # Vasco-like postprocess on same HEF (via Vasco wrapper script)
    python3 "$SCRIPT_VASCO" \
    --hef "$HEF" \
    --source "$BASE/videos/$v" \
    --output "$RESULTS/${vname}_yolov8s_detect_vascolike.mp4" \
    --model-type detect \
    --model-name yolov8s_detect_vascolike \
    --conf 0.30 --iou 0.40 \
    --min-box-size 0.04 --global-nms-iou 0.25 \
    --sync-writer --label-decimals 3

done

python3 - <<'PY'
import glob, json, os
files = sorted(glob.glob('/data/yolo_benchmark/results/fase_f_comparacao_com_vasco/*_stats.json'))
rows = []
for p in files:
    d = json.load(open(p))
    bn = os.path.basename(p)
    if '_baseline_' in bn:
        mode = 'baseline'
    elif '_vascolike_' in bn:
        mode = 'vascolike'
    else:
        mode = 'unknown'
    rows.append((
        os.path.basename(str(d.get('source', 'unknown'))).replace('.mp4',''),
        mode,
        d.get('real_fps'),
        d.get('pipeline_fps_stage_total'),
        d.get('mean_conf'),
        d.get('p50_conf'),
        d.get('p95_conf'),
        d.get('conf_thresh'),
        d.get('iou_thresh'),
        d.get('min_box_size'),
        d.get('global_nms_iou'),
    ))
rows.sort(key=lambda x: (x[0], x[1]))
print('\nvideo   mode        real_fps  pipe_stage  mean_conf  p50_conf  p95_conf  conf   iou  min_box global_nms')
print('---------------------------------------------------------------------------------------------------------')
for r in rows:
    v,m,rf,pf,mc,p50,p95,c,iou,mb,gn = r
    def f(x,n):
        return 'NA' if x is None else f'{x:.{n}f}'
    print(f'{v:<7} {m:<10} {f(rf,2):>8} {f(pf,2):>11} {f(mc,4):>10} {f(p50,4):>9} {f(p95,4):>9} {f(c,2):>6} {f(iou,2):>5} {f(mb,3):>7} {f(gn,2):>10}')
print(f'\nTotal rows: {len(rows)}')
PY
BASH
