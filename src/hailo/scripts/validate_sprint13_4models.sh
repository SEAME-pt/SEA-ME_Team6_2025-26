#!/usr/bin/env bash
# Sprint 13 — Phase D: Validation metrics (mAP, Precision, Recall)
# Runs `yolo val` on each model to produce quality metrics tables

set -euo pipefail

# ─── Paths ───────────────────────────────────────────────────────────────────
RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
CFG_DIR="/home/seame/Documents/AI/Yolo_benchmark/sprint13_configs"

# Trained models (from Phase B)
PT_YOLO8S_DET="$RESULTS_DIR/yolov8s_detect_sprint13/weights/best.pt"
PT_YOLO26N_DET="$RESULTS_DIR/yolo26n_detect_sprint13/weights/best.pt"
PT_YOLO8N_SEG="$RESULTS_DIR/yolov8n_seg_sprint13/weights/best.pt"
PT_YOLO26N_SEG="$RESULTS_DIR/yolo26n_seg_sprint13/weights/best.pt"

# YAML configs (for validation split)
YAML_YOLO8_DET="$CFG_DIR/data_yolo8_detect.yaml"
YAML_YOLO26_DET="$CFG_DIR/data_yolo26_detect.yaml"
YAML_YOLO8_SEG="$CFG_DIR/data_yolo8seg.yaml"
YAML_YOLO26_SEG="$CFG_DIR/data_yolo26seg.yaml"

PYTHON="python3"
DEVICE=0

# ─── Setup ───────────────────────────────────────────────────────────────────
DRY_RUN=0
[[ "${1:-}" == "--dry-run" ]] && DRY_RUN=1

# Create subdirectory for Phase D outputs
PHASE_D_DIR="$RESULTS_DIR/phase_d_validation"
mkdir -p "$PHASE_D_DIR"

TS=$(date +%Y%m%d_%H%M%S)
LOG="$PHASE_D_DIR/validate_${TS}.log"

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "$LOG"; }

log "=== Sprint 13 — Phase D: Validation Metrics ==="
log "Results dir : $PHASE_D_DIR"
log "Dry-run     : $DRY_RUN"

# ─── Pre-flight checks ───────────────────────────────────────────────────────
ERRORS=0
for label_path in \
    "PT_YOLO8S_DET:$PT_YOLO8S_DET" \
    "PT_YOLO26N_DET:$PT_YOLO26N_DET" \
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

# ─── Validation helper ───────────────────────────────────────────────────────
run_val() {
    local task="$1"           # detect | segment
    local model_pt="$2"
    local data_yaml="$3"
    local model_name="$4"     # e.g., yolov8s_detect
    local idx="$5"

    log ""
    log "────────────────────────────────────────"
    log "Validate $idx/4: $model_name  (task=$task)"
    log "  model : $model_pt"
    log "  data  : $data_yaml"
    log "────────────────────────────────────────"

    if [[ $DRY_RUN -eq 1 ]]; then
        log "[DRY-RUN] skipping actual validation"
        return
    fi

    local t_start
    t_start=$(date +%s)

    $PYTHON - <<PYEOF 2>&1 | tee -a "$LOG"
from ultralytics import YOLO
import json
import os
import shutil

m = YOLO("$model_pt")
results = m.val(
    data="$data_yaml",
    task="$task",
    device=$DEVICE,
    imgsz=640,
    batch=16,
    verbose=True,
)

def as_float(v):
    try:
        return float(v)
    except Exception:
        return None

results_dict = getattr(results, "results_dict", {}) or {}

# Extract key metrics using Ultralytics standard keys from results_dict.
metrics_dict = {
    "model": os.path.basename("$model_pt"),
    "task": "$task",
    "fitness": as_float(getattr(results, "fitness", None)),
}

if "$task" == "detect":
    metrics_dict.update({
        "map50": as_float(results_dict.get("metrics/mAP50(B)")),
        "map50_95": as_float(results_dict.get("metrics/mAP50-95(B)")),
        "precision": as_float(results_dict.get("metrics/precision(B)")),
        "recall": as_float(results_dict.get("metrics/recall(B)")),
    })
elif "$task" == "segment":
    metrics_dict.update({
        "map50_box": as_float(results_dict.get("metrics/mAP50(B)")),
        "map50_95_box": as_float(results_dict.get("metrics/mAP50-95(B)")),
        "precision_box": as_float(results_dict.get("metrics/precision(B)")),
        "recall_box": as_float(results_dict.get("metrics/recall(B)")),
        "map50_mask": as_float(results_dict.get("metrics/mAP50(M)")),
        "map50_95_mask": as_float(results_dict.get("metrics/mAP50-95(M)")),
        "precision_mask": as_float(results_dict.get("metrics/precision(M)")),
        "recall_mask": as_float(results_dict.get("metrics/recall(M)")),
    })

# Save metrics JSON
json_out = "$PHASE_D_DIR/${model_name}_val_metrics.json"
with open(json_out, "w") as f:
    json.dump(metrics_dict, f, indent=2)
print(f"Saved metrics: {json_out}")

# Save full Ultralytics validation logs folder if available.
results_dir = str(getattr(results, "save_dir", ""))
if results_dir and os.path.exists(results_dir):
    shutil.rmtree("$PHASE_D_DIR/../${model_name}_val_logs", ignore_errors=True)
    shutil.copytree(results_dir, "$PHASE_D_DIR/../${model_name}_val_logs")
    print(f"Saved val logs: $PHASE_D_DIR/../${model_name}_val_logs")

# Cleanup temp ultralytics runs folder
shutil.rmtree("runs", ignore_errors=True)
PYEOF

    local t_end exit_code=$?
    t_end=$(date +%s)
    local elapsed=$(( (t_end - t_start) / 60 ))

    if [[ $exit_code -ne 0 ]]; then
        log "ERROR: validation failed (exit $exit_code)"
        return $exit_code
    fi

    log "OK in ${elapsed} min"
}

# ─── 4 validation runs ────────────────────────────────────────────────────────
T_GLOBAL_START=$(date +%s)

run_val "detect"  "$PT_YOLO8S_DET"   "$YAML_YOLO8_DET"   "yolov8s_detect"  1
run_val "detect"  "$PT_YOLO26N_DET"  "$YAML_YOLO26_DET"  "yolo26n_detect"  2
run_val "segment" "$PT_YOLO8N_SEG"   "$YAML_YOLO8_SEG"   "yolov8n_seg"     3
run_val "segment" "$PT_YOLO26N_SEG"  "$YAML_YOLO26_SEG"  "yolo26n_seg"     4

# ─── Summary ──────────────────────────────────────────────────────────────────
T_GLOBAL_END=$(date +%s)
TOTAL_MIN=$(( (T_GLOBAL_END - T_GLOBAL_START) / 60 ))

log ""
log "=== Phase D complete (total: ${TOTAL_MIN} min) ==="
log ""
log "Validation metrics saved to: $PHASE_D_DIR"
python3 - <<PYEOF 2>&1 | tee -a "$LOG"
import os, json
phase_d = "$PHASE_D_DIR"
for f in sorted(os.listdir(phase_d)):
    if f.endswith("_val_metrics.json"):
        with open(os.path.join(phase_d, f)) as fh:
            data = json.load(fh)
        print(f"\n{f}:")
        for k, v in data.items():
            if v is not None:
                v_fmt = f"{v:.4f}" if isinstance(v, float) else str(v)
                print(f"  {k}: {v_fmt}")
PYEOF
