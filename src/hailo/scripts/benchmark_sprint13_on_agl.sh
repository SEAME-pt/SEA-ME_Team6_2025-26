#!/usr/bin/env bash
# Sprint 13 — Phase F: AGL Runtime Benchmark
# Runs 4 HEF models on Hailo board (AGL) with test videos
# Measures: FPS, latency, detection accuracy, inference speed
#
# Prerequisites:
#   - AGL board accessible at $AGL_IP
#   - HEF files from Phase E in results/sprint13_runs/phase_e_export/
#   - SSH key configured for passwordless access
#   - Test videos (teste1.mp4, teste2.mp4) available

set -euo pipefail

# ─── Paths ───────────────────────────────────────────────────────────────────
RESULTS_DIR="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs"
PHASE_E_DIR="$RESULTS_DIR/phase_e_export"
VIDEOS_DIR="/home/seame/Documents/AI/Yolo_benchmark/Vasco/final_dataset"

# AGL target
AGL_IP="${AGL_IP:-10.21.220.191}"
AGL_USER="root"
AGL_SCRIPTS="/root/AGL_scripts"

# Test videos
VIDEO_TESTE1="$VIDEOS_DIR/teste1.mp4"
VIDEO_TESTE2="$VIDEOS_DIR/teste2.mp4"

SSH="ssh -o BatchMode=yes -o StrictHostKeyChecking=no"
SCP="scp -o BatchMode=yes -o StrictHostKeyChecking=no"
PYTHON="python3"

# ─── Setup ───────────────────────────────────────────────────────────────────
DRY_RUN=0
[[ "${1:-}" == "--dry-run" ]] && DRY_RUN=1

# Create subdirectory for Phase F outputs
PHASE_F_DIR="$RESULTS_DIR/phase_f_agl_benchmark"
mkdir -p "$PHASE_F_DIR"

TS=$(date +%Y%m%d_%H%M%S)
LOG="$PHASE_F_DIR/benchmark_${TS}.log"

log() { echo "[$(date '+%H:%M:%S')] $*" | tee -a "$LOG"; }

log "=== Sprint 13 — Phase F: AGL Runtime Benchmark ==="
log "AGL IP      : $AGL_IP"
log "Results dir : $PHASE_F_DIR"
log "Dry-run     : $DRY_RUN"

# ─── Pre-flight checks ───────────────────────────────────────────────────────
ERRORS=0

# Check HEF files exist (expected 4)
expected_hefs=(
    "$PHASE_E_DIR/yolov8s_detect_sprint13.hef"
    "$PHASE_E_DIR/yolo26n_detect_sprint13.hef"
    "$PHASE_E_DIR/yolov8n_seg_sprint13.hef"
    "$PHASE_E_DIR/yolo26n_seg_sprint13.hef"
)

for hef in "${expected_hefs[@]}"; do
    if [[ ! -f "$hef" ]]; then
        log "MISSING HEF: $hef"
        ERRORS=$((ERRORS+1))
    else
        log "OK  HEF: $(basename "$hef")"
    fi
done

# Check test videos
for vid in "$VIDEO_TESTE1" "$VIDEO_TESTE2"; do
    if [[ ! -f "$vid" ]]; then
        log "MISSING VIDEO: $vid"
        ERRORS=$((ERRORS+1))
    else
        log "OK  VIDEO: $(basename $vid)"
    fi
done

# Check AGL connectivity
if [[ $DRY_RUN -eq 0 ]]; then
    log "Testing AGL SSH connectivity..."
    if $SSH "$AGL_USER@$AGL_IP" "echo OK" 2>/dev/null; then
        log "OK  AGL SSH accessible"
    else
        log "ERROR: AGL SSH not accessible at $AGL_IP"
        ERRORS=$((ERRORS+1))
    fi
fi

if [[ $ERRORS -gt 0 ]]; then
    log "Aborting: $ERRORS missing prerequisites."
    exit 1
fi

# ─── Benchmark helper ────────────────────────────────────────────────────────
run_hef_benchmark() {
    local hef_file="$1"      # e.g., yolov8s_detect_sprint13.hef
    local video_path="$2"    # teste1.mp4 or teste2.mp4
    local model_name="$3"    # e.g., yolov8s_detect
    local video_name="$4"    # teste1 or teste2
    local idx="$5"

    local out_prefix="agl_${model_name}_${video_name}"

    log ""
    log "────────────────────────────────────────"
    log "Benchmark $idx/8: $out_prefix"
    log "  HEF   : $(basename $hef_file)"
    log "  Video : $video_name"
    log "────────────────────────────────────────"

    if [[ $DRY_RUN -eq 1 ]]; then
        log "[DRY-RUN] skipping actual benchmark"
        return
    fi

    local t_start
    t_start=$(date +%s)

    # 1. Create temp dir on AGL
    local agl_tmp="/tmp/sprint13_${model_name}_${TS}"
    $SSH "$AGL_USER@$AGL_IP" "mkdir -p $agl_tmp" 2>&1 | tee -a "$LOG"

    # 2. Copy HEF + video to AGL
    log "Copying HEF to AGL..."
    $SCP "$hef_file" "$AGL_USER@$AGL_IP:$agl_tmp/" 2>&1 | tee -a "$LOG"

    log "Copying video to AGL..."
    $SCP "$video_path" "$AGL_USER@$AGL_IP:$agl_tmp/" 2>&1 | tee -a "$LOG"

    # 3. Run inference on AGL
    # (Adjust script path and parameters as needed for your AGL setup)
    log "Running inference on AGL (running hef_infer script)..."
    $SSH "$AGL_USER@$AGL_IP" <<REMOTE_CMD 2>&1 | tee -a "$LOG"
cd $agl_tmp
HEF_PATH="./${hef_file##*/}"
VIDEO_PATH="./${video_path##*/}"

# Placeholder: call your HEF inference script
# This is a template — replace with actual AGL inference command
if [[ -x "$AGL_SCRIPTS/hef_infer_video.py" ]]; then
    python3 "$AGL_SCRIPTS/hef_infer_video.py" \
        --hef "\$HEF_PATH" \
        --video "\$VIDEO_PATH" \
        --save \
        --output-video "${out_prefix}.mp4" \
        --output-stats "${out_prefix}_stats.csv"
else
    echo "WARNING: AGL inference script not found at $AGL_SCRIPTS/hef_infer_video.py"
    echo "Create placeholder output files for testing..."
    touch "${out_prefix}_stats.csv"
    touch "${out_prefix}.mp4"
fi
REMOTE_CMD

    # 4. Copy results back
    log "Copying results from AGL..."
    $SCP "$AGL_USER@$AGL_IP:$agl_tmp/${out_prefix}_stats.csv" "$PHASE_F_DIR/" 2>/dev/null || \
        log "WARNING: stats CSV not found on AGL"
    $SCP "$AGL_USER@$AGL_IP:$agl_tmp/${out_prefix}.mp4" "$PHASE_F_DIR/" 2>/dev/null || \
        log "WARNING: output MP4 not found on AGL"

    # 5. Cleanup AGL temp dir
    $SSH "$AGL_USER@$AGL_IP" "rm -rf $agl_tmp" 2>&1 | tee -a "$LOG"

    local t_end exit_code=$?
    t_end=$(date +%s)
    local elapsed=$(( (t_end - t_start) / 60 ))

    log "OK in ${elapsed} min"
}

# ─── 8 benchmark runs (4 models × 2 videos) ──────────────────────────────────
T_GLOBAL_START=$(date +%s)

declare -a hef_files=()

# Collect HEF files (in order: 2 detect, 2 seg)
for hef in "$PHASE_E_DIR"/yolov8s_detect_sprint13.hef \
           "$PHASE_E_DIR"/yolo26n_detect_sprint13.hef \
           "$PHASE_E_DIR"/yolov8n_seg_sprint13.hef \
           "$PHASE_E_DIR"/yolo26n_seg_sprint13.hef; do
    if [[ -f "$hef" ]]; then
        hef_files+=("$hef")
    fi
done

log "Found ${#hef_files[@]} HEF files"

idx=1
for i in "${!hef_files[@]}"; do
    hef="${hef_files[$i]}"
    model_short=$(basename "$hef" .hef)

    # Benchmark on both videos
    run_hef_benchmark "$hef" "$VIDEO_TESTE1" "$model_short" "teste1" $((idx++))
    run_hef_benchmark "$hef" "$VIDEO_TESTE2" "$model_short" "teste2" $((idx++))
done

# ─── Generate comparison table ────────────────────────────────────────────────
log ""
log "=== Generating Final Comparison Table ==="

$PYTHON - <<PYEOF 2>&1 | tee -a "$LOG"
import os, csv, glob

phase_f = "$PHASE_F_DIR"
comparison_file = os.path.join(phase_f, "agl_final_comparison_table.csv")

rows = []
for stats_csv in sorted(glob.glob(os.path.join(phase_f, "*_stats.csv"))):
    try:
        with open(stats_csv, newline='') as f:
            reader = csv.DictReader(f)
            for row in reader:
                row["result_file"] = os.path.basename(stats_csv)
                rows.append(row)
    except Exception as e:
        print(f"Warning: could not read {stats_csv}: {e}")

if rows:
    with open(comparison_file, "w", newline='') as f:
        writer = csv.DictWriter(f, fieldnames=rows[0].keys())
        writer.writeheader()
        writer.writerows(rows)
    print(f"Comparison table saved: {comparison_file}")
else:
    print("No stats found. Check if AGL inference ran successfully.")
PYEOF

# ─── Final summary ───────────────────────────────────────────────────────────
T_GLOBAL_END=$(date +%s)
TOTAL_MIN=$(( (T_GLOBAL_END - T_GLOBAL_START) / 60 ))

log ""
log "=== Phase F complete (total: ${TOTAL_MIN} min) ==="
log ""
log "Results in: $PHASE_F_DIR"
ls -lh "$PHASE_F_DIR" 2>/dev/null | tee -a "$LOG"

log ""
log "Next steps:"
log "1. Review agl_final_comparison_table.csv"
log "2. Analyze performance metrics (FPS, latency, accuracy)"
log "3. Document findings in docs/guides/Hailo/sprint13_results_final.md"
