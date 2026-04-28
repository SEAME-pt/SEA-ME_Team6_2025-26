#!/usr/bin/env bash
set -euo pipefail

# Train our YOLOv8s using Vasco's hyperparameters and compare both models on the same test split.

DATA_YAML="${DATA_YAML:-/home/seame/Documents/AI/Yolo_benchmark/roboflow_dataset/1381_img/object labels.v2i.yolov8/data.yaml}"
VASCO_ARGS_YAML="${VASCO_ARGS_YAML:-/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset/runs/detect/train2/args.yaml}"
VASCO_WEIGHTS="${VASCO_WEIGHTS:-/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset/runs/detect/train2/weights/best.pt}"

WORK_ROOT="${WORK_ROOT:-/home/seame/Documents/AI/Yolo_benchmark}"
TRAIN_PROJECT="${TRAIN_PROJECT:-${WORK_ROOT}/runs}"
TRAIN_NAME="${TRAIN_NAME:-ours_yolov8s_like_vasco}"
COMPARE_PROJECT="${COMPARE_PROJECT:-${WORK_ROOT}/compare}"

# Exact parameters from Vasco's train2 args.yaml
EPOCHS="${EPOCHS:-100}"
BATCH="${BATCH:-4}"
IMGSZ="${IMGSZ:-640}"
DEVICE="${DEVICE:-0}"
WORKERS="${WORKERS:-2}"
SEED="${SEED:-0}"
PATIENCE="${PATIENCE:-50}"
SKIP_TRAIN="${SKIP_TRAIN:-0}"
VAL_SPLIT="${VAL_SPLIT:-test}"

command -v yolo >/dev/null 2>&1 || {
  echo "ERROR: 'yolo' command not found. Install ultralytics first." >&2
  exit 1
}

[[ -f "$DATA_YAML" ]] || { echo "ERROR: DATA_YAML not found: $DATA_YAML" >&2; exit 1; }
[[ -f "$VASCO_ARGS_YAML" ]] || { echo "ERROR: VASCO_ARGS_YAML not found: $VASCO_ARGS_YAML" >&2; exit 1; }
[[ -f "$VASCO_WEIGHTS" ]] || { echo "ERROR: VASCO_WEIGHTS not found: $VASCO_WEIGHTS" >&2; exit 1; }

mkdir -p "$TRAIN_PROJECT" "$COMPARE_PROJECT"

extract_all_metrics_line() {
  local log_path="$1"
  awk '/^[[:space:]]*all[[:space:]]+[0-9]+[[:space:]]+[0-9]+[[:space:]]+[0-9.]+[[:space:]]+[0-9.]+[[:space:]]+[0-9.]+[[:space:]]+[0-9.]+/ { line=$0 } END { print line }' "$log_path"
}

echo "[1/4] Training our YOLOv8s with Vasco-matched setup..."
OUR_WEIGHTS="$TRAIN_PROJECT/$TRAIN_NAME/weights/best.pt"
if [[ "$SKIP_TRAIN" == "1" ]]; then
  echo "Skipping training and reusing existing weights: $OUR_WEIGHTS"
  [[ -f "$OUR_WEIGHTS" ]] || { echo "ERROR: SKIP_TRAIN=1 but weights not found: $OUR_WEIGHTS" >&2; exit 1; }
else
  yolo detect train \
    model=yolov8s.pt \
    data="$DATA_YAML" \
    epochs="$EPOCHS" \
    batch="$BATCH" \
    imgsz="$IMGSZ" \
    device="$DEVICE" \
    workers="$WORKERS" \
    seed="$SEED" \
    deterministic=True \
    optimizer=auto \
    close_mosaic=10 \
    patience="$PATIENCE" \
    amp=True \
    project="$TRAIN_PROJECT" \
    name="$TRAIN_NAME"
  [[ -f "$OUR_WEIGHTS" ]] || { echo "ERROR: our best.pt not found: $OUR_WEIGHTS" >&2; exit 1; }
fi

VASCO_VAL_LOG="$COMPARE_PROJECT/vasco_pt/val_output.log"
OURS_VAL_LOG="$COMPARE_PROJECT/ours_pt/val_output.log"
SUMMARY_TXT="$COMPARE_PROJECT/comparison_summary.txt"

echo "[2/4] Validating Vasco model on split='${VAL_SPLIT}'..."
yolo detect val \
  model="$VASCO_WEIGHTS" \
  data="$DATA_YAML" \
  split="$VAL_SPLIT" \
  imgsz="$IMGSZ" \
  batch="$BATCH" \
  device="$DEVICE" \
  project="$COMPARE_PROJECT" \
  name=vasco_pt | tee "$VASCO_VAL_LOG"

echo "[3/4] Validating our model on split='${VAL_SPLIT}'..."
yolo detect val \
  model="$OUR_WEIGHTS" \
  data="$DATA_YAML" \
  split="$VAL_SPLIT" \
  imgsz="$IMGSZ" \
  batch="$BATCH" \
  device="$DEVICE" \
  project="$COMPARE_PROJECT" \
  name=ours_pt | tee "$OURS_VAL_LOG"

VASCO_CSV="$COMPARE_PROJECT/vasco_pt/results.csv"
OURS_CSV="$COMPARE_PROJECT/ours_pt/results.csv"
VASCO_METRICS_LINE="$(extract_all_metrics_line "$VASCO_VAL_LOG")"
OURS_METRICS_LINE="$(extract_all_metrics_line "$OURS_VAL_LOG")"

echo "[4/4] Comparison summary"
echo "- Vasco args source: $VASCO_ARGS_YAML"
echo "- Vasco weights:     $VASCO_WEIGHTS"
echo "- Our weights:       $OUR_WEIGHTS"
echo "- Vasco val csv:     $VASCO_CSV"
echo "- Our val csv:       $OURS_CSV"
echo "- Summary file:      $SUMMARY_TXT"

echo
if [[ -f "$VASCO_CSV" ]]; then
  echo "Vasco metrics (last row):"
  tail -n 1 "$VASCO_CSV"
elif [[ -n "$VASCO_METRICS_LINE" ]]; then
  echo "Vasco metrics (from val log):"
  echo "$VASCO_METRICS_LINE"
else
  echo "WARNING: missing $VASCO_CSV"
fi

echo
if [[ -f "$OURS_CSV" ]]; then
  echo "Our metrics (last row):"
  tail -n 1 "$OURS_CSV"
elif [[ -n "$OURS_METRICS_LINE" ]]; then
  echo "Our metrics (from val log):"
  echo "$OURS_METRICS_LINE"
else
  echo "WARNING: missing $OURS_CSV"
fi

cat > "$SUMMARY_TXT" <<EOF
YOLOv8s comparison summary

Vasco args source: $VASCO_ARGS_YAML
Vasco weights:     $VASCO_WEIGHTS
Our weights:       $OUR_WEIGHTS
Vasco val log:     $VASCO_VAL_LOG
Our val log:       $OURS_VAL_LOG
Vasco val csv:     $VASCO_CSV
Our val csv:       $OURS_CSV

Vasco metrics:
${VASCO_METRICS_LINE:-missing}

Our metrics:
${OURS_METRICS_LINE:-missing}
EOF

echo
cat <<EOF
Done.
Summary written to: $SUMMARY_TXT
If you want HEF-vs-HEF runtime comparison next, run the same Hailo inference script twice
(changing only HEF_PATH) and compare generated *_stats.csv files.
EOF
