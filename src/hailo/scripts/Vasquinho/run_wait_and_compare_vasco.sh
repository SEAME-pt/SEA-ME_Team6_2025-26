#!/usr/bin/env bash
# Wait for Vasco-style HEFs to be generated, then run comparison

set -euo pipefail

PHASE_E="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export"
REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
SCRIPTS_DIR="$REPO/src/hailo/scripts/Vasquinho"

MODELS=(
  "yolov8s_detect"
  "yolo26n_detect"
  "yolov8n_seg"
  "yolo26n_seg"
)

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║ WAITING FOR VASCO-STYLE HEFs + AUTO-COMPARE                   ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "Polling: $PHASE_E"
echo ""

# Wait for all HEF files
TIMEOUT=1800  # 30 minutes
ELAPSED=0
POLL_INTERVAL=10

while [[ $ELAPSED -lt $TIMEOUT ]]; do
    ALL_READY=1
    for MODEL in "${MODELS[@]}"; do
        HEF="$PHASE_E/${MODEL}_sprint13_vasco.hef"
        if [[ ! -f "$HEF" ]]; then
            ALL_READY=0
            break
        fi
    done
    
    if [[ $ALL_READY -eq 1 ]]; then
        echo "✅ All Vasco-style HEFs ready!"
        break
    fi
    
    echo "[$(date +'%H:%M:%S')] Waiting... ($ELAPSED/$TIMEOUT sec)"
    sleep $POLL_INTERVAL
    ELAPSED=$((ELAPSED + POLL_INTERVAL))
done

if [[ $ALL_READY -ne 1 ]]; then
    echo "❌ Timeout waiting for HEF files"
    exit 1
fi

echo ""
echo "Generated HEFs:"
ls -lh "$PHASE_E"/*_sprint13_vasco.hef

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║ RUNNING COMPARISON: ONNX vs Vasco-style HEF                    ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Run comparison with HEF_VARIANT_SUFFIX=_vasco
cd "$REPO"
VIDEO_NAME=teste1 MAX_FRAMES=30 HEF_VARIANT_SUFFIX=_vasco CONF_DETECT=0.45 CONF_SEG=0.25 \
    bash "$SCRIPTS_DIR/run_compare_all4_onnx_vs_hef.sh"

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║ ✅ COMPARISON COMPLETE                                         ║"
echo "║ Results: .../phase_f_agl_rerun/compare_onnx_vs_hef_hostdecode/ ║"
echo "╚════════════════════════════════════════════════════════════════╝"
