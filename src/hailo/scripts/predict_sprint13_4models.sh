#!/usr/bin/env bash
# Sprint 13 — Phase C: predict on test videos (teste1 + teste2)
# Generates annotated MP4 + detection stats JSON for each model/video combo
#
# Setup:
#   - 4 best.pt models from Phase B training
#   - 2 input test videos (teste1.mp4, teste2.mp4)
#   - 8 outputs: 4 models × 2 videos × (mp4 + json stats)

set -euo pipefail

# ─── Paths ───────────────────────────────────────────────────────────────────
RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
VIDEOS_DIR="/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset"

# Trained models (from Phase B)
PT_YOLO8S_DET="$RESULTS_DIR/yolov8s_detect_sprint13/weights/best.pt"
PT_YOLO26N_DET="$RESULTS_DIR/yolo26n_detect_sprint13/weights/best.pt"
PT_YOLO8N_SEG="$RESULTS_DIR/yolov8n_seg_sprint13/weights/best.pt"
PT_YOLO26N_SEG="$RESULTS_DIR/yolo26n_seg_sprint13/weights/best.pt"

# Test videos
VIDEO_TESTE1="$VIDEOS_DIR/teste1.mp4"
VIDEO_TESTE2="$VIDEOS_DIR/teste2.mp4"

PYTHON="python3"
DEVICE=0  # GPU 0

# ─── Setup ───────────────────────────────────────────────────────────────────
DRY_RUN=0
[[ "${1:-}" == "--dry-run" ]] && DRY_RUN=1

# Create subdirectory for Phase C outputs
PHASE_C_DIR="$RESULTS_DIR/phase_c_predictions"
mkdir -p "$PHASE_C_DIR"

TS=$(date +%Y%m%d_%H%M%S)
LOG="$PHASE_C_DIR/predict_${TS}.log"

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "$LOG"; }

log "=== Sprint 13 — Phase C: Predict on Test Videos ==="
log "Results dir : $PHASE_C_DIR"
log "Dry-run     : $DRY_RUN"

# ─── Pre-flight checks ───────────────────────────────────────────────────────
ERRORS=0
for label_path in \
    "PT_YOLO8S_DET:$PT_YOLO8S_DET" \
    "PT_YOLO26N_DET:$PT_YOLO26N_DET" \
    "PT_YOLO8N_SEG:$PT_YOLO8N_SEG" \
    "PT_YOLO26N_SEG:$PT_YOLO26N_SEG" \
    "VIDEO_TESTE1:$VIDEO_TESTE1" \
    "VIDEO_TESTE2:$VIDEO_TESTE2"
do
    label="${label_path%%:*}"
    path="${label_path#*:}"
    if [[ ! -f "$path" ]]; then
        log "MISSING $label: $path"
        ERRORS=$((ERRORS+1))
    else
        log "OK  $label"
    fi
done

if [[ $ERRORS -gt 0 ]]; then
    log "Aborting: $ERRORS missing file(s)."
    exit 1
fi

# ─── Predict helper ──────────────────────────────────────────────────────────
run_predict() {
    local task="$1"           # detect | segment
    local model_pt="$2"
    local video_path="$3"
    local out_prefix="$4"     # e.g., yolov8s_detect_teste1
    local idx="$5"

    log ""
    log "────────────────────────────────────────"
    log "Predict $idx/8: $out_prefix"
    log "  model : $model_pt"
    log "  video : $video_path"
    log "  task  : $task"
    log "────────────────────────────────────────"

    if [[ $DRY_RUN -eq 1 ]]; then
        log "[DRY-RUN] skipping actual prediction"
        return
    fi

    local t_start
    t_start=$(date +%s)

    $PYTHON - <<PYEOF 2>&1 | tee -a "$LOG"
from ultralytics import YOLO
import json
import cv2
import os

m = YOLO("$model_pt")
results = m.predict(
    source="$video_path",
    task="$task",
    device=$DEVICE,
    imgsz=640,
    conf=0.25,
    save=True,
    save_txt=False,
    save_conf=True,
    verbose=False,
)

# Extract stats from results
stats = {
    "model": os.path.basename("$model_pt"),
    "task": "$task",
    "video": os.path.basename("$video_path"),
    "total_frames": len(results),
    "frames_with_detections": sum(1 for r in results if len(r.boxes) > 0),
    "total_detections": sum(len(r.boxes) for r in results),
}

# Save stats JSON
json_out = "$PHASE_C_DIR/${out_prefix}_stats.json"
with open(json_out, "w") as f:
    json.dump(stats, f, indent=2)
print(f"Saved stats: {json_out}")

# Move video from runs/predict/videos to phase_c
import shutil
src_vid = f"runs/detect/{os.path.basename('$model_pt').replace('.pt', '')}/video.mp4"
dst_vid = "$PHASE_C_DIR/${out_prefix}.mp4"
if os.path.exists(src_vid):
    shutil.move(src_vid, dst_vid)
    print(f"Saved video: {dst_vid}")

# Cleanup temp ultralytics runs folder
shutil.rmtree("runs", ignore_errors=True)
PYEOF

    local t_end exit_code=$?
    t_end=$(date +%s)
    local elapsed=$(( (t_end - t_start) / 60 ))

    if [[ $exit_code -ne 0 ]]; then
        log "ERROR: prediction failed for $out_prefix (exit $exit_code)"
        return $exit_code
    fi

    log "OK in ${elapsed} min"
}

# ─── 8 prediction runs (4 models × 2 videos) ─────────────────────────────────
T_GLOBAL_START=$(date +%s)

# Detect
run_predict "detect" "$PT_YOLO8S_DET"   "$VIDEO_TESTE1" "yolov8s_detect_teste1" 1
run_predict "detect" "$PT_YOLO8S_DET"   "$VIDEO_TESTE2" "yolov8s_detect_teste2" 2
run_predict "detect" "$PT_YOLO26N_DET"  "$VIDEO_TESTE1" "yolo26n_detect_teste1" 3
run_predict "detect" "$PT_YOLO26N_DET"  "$VIDEO_TESTE2" "yolo26n_detect_teste2" 4

# Segment
run_predict "segment" "$PT_YOLO8N_SEG"   "$VIDEO_TESTE1" "yolov8n_seg_teste1" 5
run_predict "segment" "$PT_YOLO8N_SEG"   "$VIDEO_TESTE2" "yolov8n_seg_teste2" 6
run_predict "segment" "$PT_YOLO26N_SEG"  "$VIDEO_TESTE1" "yolo26n_seg_teste1" 7
run_predict "segment" "$PT_YOLO26N_SEG"  "$VIDEO_TESTE2" "yolo26n_seg_teste2" 8

# ─── Summary ──────────────────────────────────────────────────────────────────
T_GLOBAL_END=$(date +%s)
TOTAL_MIN=$(( (T_GLOBAL_END - T_GLOBAL_START) / 60 ))

log ""
log "=== Phase C complete (total: ${TOTAL_MIN} min) ==="
log ""
log "Outputs in: $PHASE_C_DIR"
ls -lh "$PHASE_C_DIR"/*.{mp4,json} 2>/dev/null | tee -a "$LOG" || log "No outputs yet (dry-run mode)"
