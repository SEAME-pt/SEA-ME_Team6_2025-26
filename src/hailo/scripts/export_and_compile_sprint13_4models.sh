#!/usr/bin/env bash
# Sprint 13 — Phase E: Export to ONNX + Hailo Compile
# Converts best.pt → best.onnx → best.hef for deployment on Hailo hardware
#
# Prerequisites:
#   - Hailo docker environment configured
#   - hailomz CLI available or accessible via docker
#
# This script:
#   1. Exports each best.pt to ONNX (opset 12, simplify)
#   2. Runs Hailo docker hailomz compile on ONNX → HEF

set -euo pipefail

# ─── Paths ───────────────────────────────────────────────────────────────────
RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
HAILO_DIR="/home/seame/Documents/AI/hailo/shared_with_docker"

# Trained models (from Phase B)
PT_YOLO8S_DET="$RESULTS_DIR/yolov8s_detect_sprint13/weights/best.pt"
PT_YOLO26N_DET="$RESULTS_DIR/yolo26n_detect_sprint13/weights/best.pt"
PT_YOLO8N_SEG="$RESULTS_DIR/yolov8n_seg_sprint13/weights/best.pt"
PT_YOLO26N_SEG="$RESULTS_DIR/yolo26n_seg_sprint13/weights/best.pt"

PYTHON="python3"
DEVICE=0
HAILO_DOCKER_IMAGE="${HAILO_DOCKER_IMAGE:-hailo8_ai_sw_suite_2025-10:1}"

# ─── Setup ───────────────────────────────────────────────────────────────────
DRY_RUN=0
[[ "${1:-}" == "--dry-run" ]] && DRY_RUN=1

# Create subdirectory for Phase E outputs
PHASE_E_DIR="$RESULTS_DIR/phase_e_export"
mkdir -p "$PHASE_E_DIR"

TS=$(date +%Y%m%d_%H%M%S)
LOG="$PHASE_E_DIR/export_${TS}.log"

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "$LOG"; }

log "=== Sprint 13 — Phase E: Export to ONNX + Hailo Compile ==="
log "Results dir : $PHASE_E_DIR"
log "Dry-run     : $DRY_RUN"
log "Docker image: $HAILO_DOCKER_IMAGE"

# ─── Pre-flight checks ───────────────────────────────────────────────────────
ERRORS=0
for label_path in \
    "PT_YOLO8S_DET:$PT_YOLO8S_DET" \
    "PT_YOLO26N_DET:$PT_YOLO26N_DET" \
    "PT_YOLO8N_SEG:$PT_YOLO8N_SEG" \
    "PT_YOLO26N_SEG:$PT_YOLO26N_SEG"
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

# ─── Export helper ───────────────────────────────────────────────────────────
run_export() {
    local model_pt="$1"
    local model_name="$2"
    local idx="$3"

    log ""
    log "────────────────────────────────────────"
    log "Export $idx/4: $model_name"
    log "  input  : $model_pt"
    log "  output : $PHASE_E_DIR/${model_name}.onnx"
    log "────────────────────────────────────────"

    if [[ $DRY_RUN -eq 1 ]]; then
        log "[DRY-RUN] skipping actual export"
        return
    fi

    local t_start
    t_start=$(date +%s)

    $PYTHON - <<PYEOF 2>&1 | tee -a "$LOG"
from ultralytics import YOLO
import os

m = YOLO("$model_pt")
onnx_path = m.export(
    format="onnx",
    imgsz=640,
    opset=12,
    simplify=True,
    device=$DEVICE,
)
print(f"ONNX exported: {onnx_path}")

# Copy to phase_e output dir
import shutil
dst = "$PHASE_E_DIR/${model_name}.onnx"
shutil.copy(onnx_path, dst)
print(f"Saved: {dst}")
PYEOF

    local t_end exit_code=$?
    t_end=$(date +%s)
    local elapsed=$(( (t_end - t_start) / 60 ))

    if [[ $exit_code -ne 0 ]]; then
        log "ERROR: export failed (exit $exit_code)"
        return $exit_code
    fi

    log "OK in ${elapsed} min"
}

# ─── Hailo compile helper ─────────────────────────────────────────────────────
run_hailo_compile() {
    local onnx_path="$1"
    local model_name="$2"
    local idx="$3"

    local hef_path="$PHASE_E_DIR/${model_name}.hef"
    local mz_model
    local extra_compile_args=""

    case "$model_name" in
        *seg*)
            mz_model="yolov8n_seg"
            extra_compile_args="--start-node-names images --end-node-names output0 output1"
            ;;
        *)
            mz_model="yolov8s"
            ;;
    esac

    log ""
    log "────────────────────────────────────────"
    log "Hailo Compile $idx/4: $model_name"
    log "  input  : $onnx_path"
    log "  output : $hef_path"
    log "────────────────────────────────────────"

    if [[ $DRY_RUN -eq 1 ]]; then
        log "[DRY-RUN] skipping actual compilation"
        return
    fi

    if [[ ! -f "$onnx_path" ]]; then
        log "ERROR: ONNX not found: $onnx_path"
        return 1
    fi

    local t_start
    t_start=$(date +%s)

    log "Attempting Hailo docker compile with Model Zoo profile: $mz_model"

    docker run --rm -u 0:0 \
        -v "$HAILO_DIR:/local/workspace/shared_with_docker" \
        -v "$PHASE_E_DIR:/phase_e" \
        "$HAILO_DOCKER_IMAGE" \
        bash -lc "
            set -e
            cd /local/workspace

            if [[ -d shared_with_docker/calibration_images_17c ]]; then
                CALIB_PATH='shared_with_docker/calibration_images_17c'
            elif [[ -d shared_with_docker/calibration_images ]]; then
                CALIB_PATH='shared_with_docker/calibration_images'
            else
                echo 'ERROR: calibration folder not found in shared_with_docker' >&2
                exit 1
            fi

            hailomz compile $mz_model \
                --ckpt /phase_e/${model_name}.onnx \
                --hw-arch hailo8 \
                --calib-path \"\$CALIB_PATH\" \
                $extra_compile_args \
                2>&1

            cp ${mz_model}.hef /phase_e/${model_name}.hef
        " 2>&1 | tee -a "$LOG" || {
        log "WARNING: Docker compile failed. Check if docker image is available."
        return 1
    }

    local t_end exit_code=$?
    t_end=$(date +%s)
    local elapsed=$(( (t_end - t_start) / 60 ))

    if [[ $exit_code -ne 0 ]]; then
        log "ERROR: Hailo compile failed (exit $exit_code)"
        return $exit_code
    fi

    if [[ -f "$hef_path" ]]; then
        log "OK in ${elapsed} min — HEF saved: $hef_path"
    else
        log "WARNING: HEF file not found after compile"
    fi
}

# ─── 4 export + compile runs ──────────────────────────────────────────────────
T_GLOBAL_START=$(date +%s)
EXPORT_FAILS=0
COMPILE_FAILS=0

for idx in 1 2 3 4; do
    case $idx in
        1)
            PT="$PT_YOLO8S_DET"
            NAME="yolov8s_detect_sprint13"
            ;;
        2)
            PT="$PT_YOLO26N_DET"
            NAME="yolo26n_detect_sprint13"
            ;;
        3)
            PT="$PT_YOLO8N_SEG"
            NAME="yolov8n_seg_sprint13"
            ;;
        4)
            PT="$PT_YOLO26N_SEG"
            NAME="yolo26n_seg_sprint13"
            ;;
    esac

    if ! run_export "$PT" "$NAME" $idx; then
        EXPORT_FAILS=$((EXPORT_FAILS+1))
        log "WARNING: export failed for $NAME; skipping compile"
        continue
    fi

    if ! run_hailo_compile "$PHASE_E_DIR/${NAME}.onnx" "$NAME" $idx; then
        COMPILE_FAILS=$((COMPILE_FAILS+1))
        log "WARNING: compile failed for $NAME; continuing with next model"
    fi
done

# ─── Summary ──────────────────────────────────────────────────────────────────
T_GLOBAL_END=$(date +%s)
TOTAL_MIN=$(( (T_GLOBAL_END - T_GLOBAL_START) / 60 ))

log ""
log "=== Phase E complete (total: ${TOTAL_MIN} min) ==="
log "Export failures : $EXPORT_FAILS"
log "Compile failures: $COMPILE_FAILS"
log ""
log "Exports in: $PHASE_E_DIR"
FOUND_ARTIFACTS=0
while IFS= read -r f; do
    [[ -n "$f" ]] || continue
    ls -lh "$f" | tee -a "$LOG"
    FOUND_ARTIFACTS=1
done < <(find "$PHASE_E_DIR" -maxdepth 1 -type f \( -name "*.onnx" -o -name "*.hef" \) | sort)

if [[ $FOUND_ARTIFACTS -eq 0 ]]; then
    log "No ONNX/HEF files found (dry-run or compilation failed)"
fi

log ""
log "IMPORTANT: If Hailo docker compile failed:"
log "1. Check: docker images | grep hailo"
log "2. If missing, install Hailo docker or configure alternate compile path"
log "3. Rerun: bash src/hailo/scripts/export_and_compile_sprint13_4models.sh"
