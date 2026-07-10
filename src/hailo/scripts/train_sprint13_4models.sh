#!/usr/bin/env bash
# Sprint 13 — Phase B: train 4 models (detect + seg) on local GPU
# Usage: bash train_sprint13_4models.sh [--dry-run]
#
# Output:
#   RESULTS_DIR/yolov8s_detect_sprint13/weights/best.pt
#   RESULTS_DIR/yolo26n_detect_sprint13/weights/best.pt
#   RESULTS_DIR/yolov8n_seg_sprint13/weights/best.pt
#   RESULTS_DIR/yolo26n_seg_sprint13/weights/best.pt
#
# Log: RESULTS_DIR/train_sprint13_<timestamp>.log
# Summary: RESULTS_DIR/train_sprint13_<timestamp>_summary.txt

set -euo pipefail

# ─── Paths ───────────────────────────────────────────────────────────────────
MODELS_DIR="/home/seame/Documents/AI/hailo/shared_with_docker/models/sprint13_models_benchmark"
CFG_DIR="/home/seame/Documents/AI/Yolo_benchmark/sprint13_configs"
RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
PYTHON="python3"

# Base weights
PT_YOLO8S="$MODELS_DIR/yolo8s/yolov8s.pt"
PT_YOLO26N="$MODELS_DIR/yolo26n/yolo26n.pt"
PT_YOLO8N_SEG="$MODELS_DIR/yolo8n-seg/yolov8n-seg.pt"
PT_YOLO26N_SEG="$MODELS_DIR/yolo26n-seg/yolo26n-seg.pt"

# YAML configs
YAML_YOLO8_DET="$CFG_DIR/data_yolo8_detect.yaml"
YAML_YOLO26_DET="$CFG_DIR/data_yolo26_detect.yaml"
YAML_YOLO8_SEG="$CFG_DIR/data_yolo8seg.yaml"
YAML_YOLO26_SEG="$CFG_DIR/data_yolo26seg.yaml"

# ─── Hyperparameters ─────────────────────────────────────────────────────────
EPOCHS=100
IMGSZ=640
BATCH=16
WORKERS=4
SEED=42
DEVICE=0   # GPU 0

# ─── Setup ───────────────────────────────────────────────────────────────────
DRY_RUN=0
[[ "${1:-}" == "--dry-run" ]] && DRY_RUN=1

TS=$(date +%Y%m%d_%H%M%S)
mkdir -p "$RESULTS_DIR"
LOG="$RESULTS_DIR/train_sprint13_${TS}.log"
SUMMARY="$RESULTS_DIR/train_sprint13_${TS}_summary.txt"

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "$LOG"; }

# ─── Pre-flight checks ───────────────────────────────────────────────────────
log "=== Sprint 13 — 4-model training ==="
log "Results dir : $RESULTS_DIR"
log "Log         : $LOG"
log "Dry-run     : $DRY_RUN"

ERRORS=0
for label_path in \
    "PT_YOLO8S:$PT_YOLO8S" \
    "PT_YOLO26N:$PT_YOLO26N" \
    "PT_YOLO8N_SEG:$PT_YOLO8N_SEG" \
    "PT_YOLO26N_SEG:$PT_YOLO26N_SEG" \
    "YAML_YOLO8_DET:$YAML_YOLO8_DET" \
    "YAML_YOLO26_DET:$YAML_YOLO26_DET" \
    "YAML_YOLO8_SEG:$YAML_YOLO8_SEG" \
    "YAML_YOLO26_SEG:$YAML_YOLO26_SEG"
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

$PYTHON -c "
import torch
assert torch.cuda.is_available(), 'CUDA not available'
print('CUDA OK — device:', torch.cuda.get_device_name(0))
" 2>&1 | tee -a "$LOG"

# ─── Train helper ─────────────────────────────────────────────────────────────
run_train() {
    local task="$1"    # detect | segment
    local model_pt="$2"
    local data_yaml="$3"
    local run_name="$4"
    local idx="$5"

    log ""
    log "────────────────────────────────────────"
    log "Train $idx/4: $run_name  (task=$task)"
    log "  model : $model_pt"
    log "  data  : $data_yaml"
    log "  output: $RESULTS_DIR/$run_name"
    log "────────────────────────────────────────"

    if [[ $DRY_RUN -eq 1 ]]; then
        log "[DRY-RUN] skipping actual training"
        return
    fi

    local t_start
    t_start=$(date +%s)

    $PYTHON - <<PYEOF 2>&1 | tee -a "$LOG"
from ultralytics import YOLO
m = YOLO("$model_pt")
m.train(
    task="$task",
    data="$data_yaml",
    epochs=$EPOCHS,
    imgsz=$IMGSZ,
    batch=$BATCH,
    workers=$WORKERS,
    seed=$SEED,
    deterministic=True,
    device=$DEVICE,
    project="$RESULTS_DIR",
    name="$run_name",
    exist_ok=True,
    verbose=True,
)
print("DONE: $run_name")
PYEOF

    local t_end exit_code=$?
    t_end=$(date +%s)
    local elapsed=$(( (t_end - t_start) / 60 ))

    if [[ $exit_code -ne 0 ]]; then
        log "ERROR: training failed for $run_name (exit $exit_code)"
        echo "FAIL $run_name  elapsed=${elapsed}min" >> "$SUMMARY"
        return $exit_code
    fi

    local best="$RESULTS_DIR/$run_name/weights/best.pt"
    if [[ -f "$best" ]]; then
        log "best.pt: $best"
        echo "OK   $run_name  elapsed=${elapsed}min  best=$best" >> "$SUMMARY"
    else
        log "WARNING: best.pt not found at expected path"
        echo "WARN $run_name  elapsed=${elapsed}min  best.pt missing" >> "$SUMMARY"
    fi
}

# ─── 4 training runs ─────────────────────────────────────────────────────────
T_GLOBAL_START=$(date +%s)

run_train "detect"  "$PT_YOLO8S"       "$YAML_YOLO8_DET"   "yolov8s_detect_sprint13"  1
run_train "detect"  "$PT_YOLO26N"      "$YAML_YOLO26_DET"  "yolo26n_detect_sprint13"  2
run_train "segment" "$PT_YOLO8N_SEG"   "$YAML_YOLO8_SEG"   "yolov8n_seg_sprint13"     3
run_train "segment" "$PT_YOLO26N_SEG"  "$YAML_YOLO26_SEG"  "yolo26n_seg_sprint13"     4

# ─── Final summary ────────────────────────────────────────────────────────────
T_GLOBAL_END=$(date +%s)
TOTAL_MIN=$(( (T_GLOBAL_END - T_GLOBAL_START) / 60 ))

log ""
log "=== Sprint 13 training complete  (total: ${TOTAL_MIN} min) ==="
log "Summary: $SUMMARY"
log ""
[[ -f "$SUMMARY" ]] && cat "$SUMMARY" | tee -a "$LOG" || log "(no summary — dry-run or no completed runs)"
