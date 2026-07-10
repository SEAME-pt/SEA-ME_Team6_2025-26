#!/usr/bin/env bash
# run_video_benchmark_sprint13.sh
# ---------------------------------------------------------------
# Run Sprint 13 offline video benchmark on AGL.
# Runs inference_video_sprint13.py for all 4 HEFs × 2 videos.
# Must be executed ON THE AGL BOARD.
#
# Usage:
#   bash /data/yolo_benchmark/scripts/run_video_benchmark_sprint13.sh
# ---------------------------------------------------------------
set -euo pipefail

SCRIPT_DIR="/data/yolo_benchmark/scripts"
HEF_DIR="/data/yolo_benchmark/models/hef/sprint13"
VIDEO_DIR="/data/yolo_benchmark/videos"
RESULTS_DIR="/data/yolo_benchmark/results/sprint13"

INFERENCE_PY="${SCRIPT_DIR}/inference_video_sprint13.py"
CONF=0.45
IOU=0.55

mkdir -p "${RESULTS_DIR}"

# ── Model definitions ──────────────────────────────────────────
# Format: "hef_filename|model_type|model_name"
MODELS=(
    "yolov8s_detect_sprint13.hef|detect|yolov8s_detect"
    "yolo26n_detect_sprint13.hef|detect|yolo26n_detect"
    "yolov8n_seg_sprint13.hef|seg|yolov8n_seg"
    "yolo26n_seg_sprint13.hef|seg|yolo26n_seg"
)

# ── Videos ────────────────────────────────────────────────────
VIDEOS=("teste1.mp4" "teste2.mp4")

# ── Run ───────────────────────────────────────────────────────
echo "============================================================"
echo "  Sprint 13 Video Benchmark — AGL Hailo-8"
echo "  HEFs   : ${HEF_DIR}"
echo "  Videos : ${VIDEO_DIR}"
echo "  Results: ${RESULTS_DIR}"
echo "============================================================"
echo ""

TOTAL=$(( ${#MODELS[@]} * ${#VIDEOS[@]} ))
RUN=0

for MODEL_DEF in "${MODELS[@]}"; do
    IFS="|" read -r HEF_FILE MODEL_TYPE MODEL_NAME <<< "${MODEL_DEF}"
    HEF_PATH="${HEF_DIR}/${HEF_FILE}"

    if [[ ! -f "${HEF_PATH}" ]]; then
        echo "[SKIP] HEF not found: ${HEF_PATH}"
        continue
    fi

    for VIDEO_FILE in "${VIDEOS[@]}"; do
        VIDEO_PATH="${VIDEO_DIR}/${VIDEO_FILE}"

        if [[ ! -f "${VIDEO_PATH}" ]]; then
            echo "[SKIP] Video not found: ${VIDEO_PATH}"
            continue
        fi

        VIDEO_STEM="${VIDEO_FILE%.mp4}"
        OUT_VIDEO="${RESULTS_DIR}/${VIDEO_STEM}_${MODEL_NAME}.mp4"

        RUN=$(( RUN + 1 ))
        echo "────────────────────────────────────────────────────────────"
        echo "  [${RUN}/${TOTAL}] ${MODEL_NAME}  ×  ${VIDEO_FILE}"
        echo "  Output: ${OUT_VIDEO}"
        echo ""

        python3 "${INFERENCE_PY}" \
            --hef        "${HEF_PATH}" \
            --source     "${VIDEO_PATH}" \
            --output     "${OUT_VIDEO}" \
            --model-type "${MODEL_TYPE}" \
            --model-name "${MODEL_NAME}" \
            --conf       "${CONF}" \
            --iou        "${IOU}"

        echo ""
        echo "  Done: ${OUT_VIDEO}"
        echo ""
    done
done

echo "============================================================"
echo "  All runs complete. Results in: ${RESULTS_DIR}"
echo "  Files:"
ls -lh "${RESULTS_DIR}"
echo "============================================================"
