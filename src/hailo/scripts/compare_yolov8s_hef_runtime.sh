#!/usr/bin/env bash
set -euo pipefail

# Compare any two HEF models on the same AGL target.
# Each side can use its own inference script, remote HEF destination path, and input frames directory.

TARGET_HOST="${TARGET_HOST:-root@10.21.220.191}"
DURATION="${DURATION:-60}"

LABEL_A="${LABEL_A:-model_a}"
LABEL_B="${LABEL_B:-model_b}"

HEF_A="${HEF_A:-}"
HEF_B="${HEF_B:-}"

REMOTE_SCRIPT_A="${REMOTE_SCRIPT_A:-/data/yolo_benchmark/scripts/yolo_realtime/inference_camera_scalercrop_yolov8s.py}"
REMOTE_SCRIPT_B="${REMOTE_SCRIPT_B:-$REMOTE_SCRIPT_A}"

REMOTE_HEF_PATH_A="${REMOTE_HEF_PATH_A:-/data/yolo_benchmark/models/hef/model_a.hef}"
REMOTE_HEF_PATH_B="${REMOTE_HEF_PATH_B:-/data/yolo_benchmark/models/hef/model_b.hef}"

IMAGES_DIR_A="${IMAGES_DIR_A:-/data/offline_round2_frames/yolov8s}"
IMAGES_DIR_B="${IMAGES_DIR_B:-$IMAGES_DIR_A}"

REMOTE_RESULTS_DIR="${REMOTE_RESULTS_DIR:-/data/yolo_benchmark/results/hef_compare_generic}"
LOCAL_RESULTS_DIR="${LOCAL_RESULTS_DIR:-/home/seame/Documents/AI/Yolo_benchmark/hef_compare_generic}"

command -v ssh >/dev/null 2>&1 || {
  echo "ERROR: 'ssh' command not found." >&2
  exit 1
}
command -v scp >/dev/null 2>&1 || {
  echo "ERROR: 'scp' command not found." >&2
  exit 1
}

[[ -n "$HEF_A" ]] || { echo "ERROR: set HEF_A path." >&2; exit 1; }
[[ -n "$HEF_B" ]] || { echo "ERROR: set HEF_B path." >&2; exit 1; }
[[ -f "$HEF_A" ]] || { echo "ERROR: HEF_A not found: $HEF_A" >&2; exit 1; }
[[ -f "$HEF_B" ]] || { echo "ERROR: HEF_B not found: $HEF_B" >&2; exit 1; }

mkdir -p "$LOCAL_RESULTS_DIR"

run_remote_benchmark() {
  local label="$1"
  local local_hef="$2"
  local remote_script="$3"
  local remote_hef_path="$4"
  local images_dir="$5"
  local remote_hef_dir
  local remote_output_mp4="$REMOTE_RESULTS_DIR/${label}.mp4"
  local remote_stats_csv="$REMOTE_RESULTS_DIR/${label}_stats.csv"
  local local_dir="$LOCAL_RESULTS_DIR/$label"

  mkdir -p "$local_dir"
  remote_hef_dir="$(dirname "$remote_hef_path")"

  echo "[${label}] Copying HEF to target..."
  scp "$local_hef" "$TARGET_HOST:$remote_hef_path"

  echo "[${label}] Running inference benchmark on target..."
  ssh "$TARGET_HOST" "mkdir -p '$REMOTE_RESULTS_DIR' '$remote_hef_dir' && HEF_PATH='$remote_hef_path' python3 '$remote_script' '$DURATION' --images-dir '$images_dir' --loop --save --output '$remote_output_mp4'"

  echo "[${label}] Copying results back..."
  scp "$TARGET_HOST:$remote_output_mp4" "$local_dir/"
  scp "$TARGET_HOST:$remote_stats_csv" "$local_dir/"
}

run_remote_benchmark "$LABEL_A" "$HEF_A" "$REMOTE_SCRIPT_A" "$REMOTE_HEF_PATH_A" "$IMAGES_DIR_A"
run_remote_benchmark "$LABEL_B" "$HEF_B" "$REMOTE_SCRIPT_B" "$REMOTE_HEF_PATH_B" "$IMAGES_DIR_B"

SUMMARY_TXT="$LOCAL_RESULTS_DIR/runtime_summary.txt"
cat > "$SUMMARY_TXT" <<EOF
HEF runtime comparison

Target host:        $TARGET_HOST
Duration:           $DURATION
Remote results dir: $REMOTE_RESULTS_DIR

Model A label:      $LABEL_A
Model A HEF:        $HEF_A
Model A script:     $REMOTE_SCRIPT_A
Model A remote HEF: $REMOTE_HEF_PATH_A
Model A images dir: $IMAGES_DIR_A

Model B label:      $LABEL_B
Model B HEF:        $HEF_B
Model B script:     $REMOTE_SCRIPT_B
Model B remote HEF: $REMOTE_HEF_PATH_B
Model B images dir: $IMAGES_DIR_B

Local results:
- $LOCAL_RESULTS_DIR/$LABEL_A/$LABEL_A.mp4
- $LOCAL_RESULTS_DIR/$LABEL_A/${LABEL_A}_stats.csv
- $LOCAL_RESULTS_DIR/$LABEL_B/$LABEL_B.mp4
- $LOCAL_RESULTS_DIR/$LABEL_B/${LABEL_B}_stats.csv
EOF

echo "Done. Summary written to: $SUMMARY_TXT"
echo "Compare the two *_stats.csv files for FPS/latency and the videos for output quality."

