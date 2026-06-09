#!/usr/bin/env bash
# Run Vasco's exact scripts (parse.py → optimize.py → compile.py) for all 4 models
# This wrapper calls Vasco's original scripts inside Docker, model by model

set -euo pipefail

DOCKER_IMAGE="${DOCKER_IMAGE:-hailo8_ai_sw_suite_2025-10:1}"
REPO="/home/seame/Documents/SEA-ME_Team6_2025-26"
SCRIPTS_DIR="$REPO/src/hailo/scripts/Vasquinho"
PHASE_E="/home/seame/Documents/AI/Yolo_benchmark/results/sprint13_runs/phase_e_export"
CALIB_DIR="/home/seame/Documents/AI/hailo/shared_with_docker/calibration_images_sprint13_detect_1024"
DOCKER_WORK="/tmp/vasco_work"

MODELS=(
  "yolov8s_detect"
  "yolo26n_detect"
  "yolov8n_seg"
  "yolo26n_seg"
)

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║ RUNNING VASCO'S SCRIPTS (parse → optimize → compile)          ║"
echo "║ For all 4 Sprint13 models                                      ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "Using Vasco's original scripts from: $SCRIPTS_DIR"
echo "Docker image: $DOCKER_IMAGE"
echo ""

for MODEL_NAME in "${MODELS[@]}"; do
    echo ""
    echo "╔════════════════════════════════════════════════════════════════╗"
    echo "║ MODEL: $MODEL_NAME"
    echo "╚════════════════════════════════════════════════════════════════╝"
    
    # yolo26n_seg uses a fixed ONNX (fan-out surgery to resolve Hailo matmul1 error)
    if [[ "$MODEL_NAME" == "yolo26n_seg" && -f "$PHASE_E/${MODEL_NAME}_sprint13_fixed.onnx" ]]; then
        ONNX_BASENAME="${MODEL_NAME}_sprint13_fixed.onnx"
    else
        ONNX_BASENAME="${MODEL_NAME}_sprint13.onnx"
    fi
    ONNX_SRC="$PHASE_E/$ONNX_BASENAME"
    HEF_OUT="$PHASE_E/${MODEL_NAME}_sprint13_vasco.hef"

        case "$MODEL_NAME" in
            yolov8s_detect)
                END_NODE_NAMES="/model.22/cv2.0/cv2.0.2/Conv,/model.22/cv3.0/cv3.0.2/Conv,/model.22/cv2.1/cv2.1.2/Conv,/model.22/cv3.1/cv3.1.2/Conv,/model.22/cv2.2/cv2.2.2/Conv,/model.22/cv3.2/cv3.2.2/Conv"
                ;;
            yolo26n_detect)
                END_NODE_NAMES="/model.23/one2one_cv2.0/one2one_cv2.0.2/Conv,/model.23/one2one_cv3.0/one2one_cv3.0.2/Conv,/model.23/one2one_cv2.1/one2one_cv2.1.2/Conv,/model.23/one2one_cv3.1/one2one_cv3.1.2/Conv,/model.23/one2one_cv2.2/one2one_cv2.2.2/Conv,/model.23/one2one_cv3.2/one2one_cv3.2.2/Conv"
                ;;
            yolov8n_seg)
                END_NODE_NAMES="/model.22/cv2.0/cv2.0.2/Conv,/model.22/cv3.0/cv3.0.2/Conv,/model.22/cv4.0/cv4.0.2/Conv,/model.22/cv2.1/cv2.1.2/Conv,/model.22/cv3.1/cv3.1.2/Conv,/model.22/cv4.1/cv4.1.2/Conv,/model.22/cv2.2/cv2.2.2/Conv,/model.22/cv3.2/cv3.2.2/Conv,/model.22/cv4.2/cv4.2.2/Conv,/model.22/proto/cv3/act/Mul"
                ;;
            yolo26n_seg)
                END_NODE_NAMES="/model.23/one2one_cv2.0/one2one_cv2.0.2/Conv,/model.23/one2one_cv3.0/one2one_cv3.0.2/Conv,/model.23/one2one_cv4.0/one2one_cv4.0.2/Conv,/model.23/one2one_cv2.1/one2one_cv2.1.2/Conv,/model.23/one2one_cv3.1/one2one_cv3.1.2/Conv,/model.23/one2one_cv4.1/one2one_cv4.1.2/Conv,/model.23/one2one_cv2.2/one2one_cv2.2.2/Conv,/model.23/one2one_cv3.2/one2one_cv3.2.2/Conv,/model.23/one2one_cv4.2/one2one_cv4.2.2/Conv,/model.23/proto/cv3/act/Mul"
                ;;
            *)
                echo "❌ Unsupported model: $MODEL_NAME"
                exit 1
                ;;
        esac
    
    if [[ ! -f "$ONNX_SRC" ]]; then
        echo "❌ ONNX not found: $ONNX_SRC"
        continue
    fi
    
    echo ""
    echo "[Step 1/3] Copy ONNX and create Docker work directory..."
    docker run --rm -u 0:0 \
        -v "$PHASE_E:$DOCKER_WORK" \
        -v "$CALIB_DIR:/calib" \
        "$DOCKER_IMAGE" \
        bash -c "
            cd $DOCKER_WORK
            cp ${ONNX_BASENAME} best.onnx
            echo '✅ Copied: best.onnx'
        "
    
    echo ""
    echo "[Step 2/3] Run parse.py (ONNX → HAR)..."
    docker run --rm -u 0:0 \
        -v "$PHASE_E:$DOCKER_WORK" \
        -v "$SCRIPTS_DIR:/scripts" \
        -e END_NODE_NAMES="$END_NODE_NAMES" \
        "$DOCKER_IMAGE" \
        bash -c "
            cd $DOCKER_WORK
            python /scripts/parse.py
            if [[ -f best.har ]]; then
                echo '✅ Generated: best.har'
            else
                echo '❌ parse.py failed'
                exit 1
            fi
        "
    
    echo ""
    echo "[Step 3/3] Run optimize.py (HAR → quantized HAR)..."
    docker run --rm -u 0:0 \
        -v "$PHASE_E:$DOCKER_WORK" \
        -v "$CALIB_DIR:/calib" \
        -v "$SCRIPTS_DIR:/scripts" \
        "$DOCKER_IMAGE" \
        bash -c "
            cd $DOCKER_WORK
            mkdir -p calibration_images
            cp /calib/*.jpg calibration_images/ 2>/dev/null || true
            if [[ ! -d calibration_images ]] || [[ -z \"\$(ls calibration_images/ 2>/dev/null)\" ]]; then
                echo '⚠️  No calib images found in calibration_images/, creating dummy set...'
                python3 << 'PY'
import cv2
import numpy as np
import os
os.makedirs('calibration_images', exist_ok=True)
for i in range(10):
    img = (np.random.rand(640, 640, 3) * 255).astype(np.uint8)
    cv2.imwrite(f'calibration_images/dummy_{i:03d}.jpg', img)
print('Created 10 dummy calib images')
PY
            fi
            python /scripts/optimize.py
            if [[ -f best_quantized.har ]]; then
                echo '✅ Generated: best_quantized.har'
            else
                echo '❌ optimize.py failed'
                exit 1
            fi
        "
    
    echo ""
    echo "[Step 4/3] Run compile.py (HAR → HEF)..."
    docker run --rm -u 0:0 \
        -v "$PHASE_E:$DOCKER_WORK" \
        -v "$SCRIPTS_DIR:/scripts" \
        "$DOCKER_IMAGE" \
        bash -c "
            cd $DOCKER_WORK
            python /scripts/compile.py
            if [[ -f best.hef ]]; then
                echo '✅ Generated: best.hef'
            else
                echo '❌ compile.py failed'
                exit 1
            fi
        "
    
    echo ""
    echo "[Finalize] Copy HEF to output and cleanup..."
    docker run --rm -u 0:0 \
        -v "$PHASE_E:$DOCKER_WORK" \
        "$DOCKER_IMAGE" \
        bash -c "
            cd $DOCKER_WORK
            if [[ -f best.hef ]]; then
                cp best.hef ${MODEL_NAME}_sprint13_vasco.hef
                rm -f best.onnx best.har best_quantized.har best.hef
                echo '✅ Saved: ${MODEL_NAME}_sprint13_vasco.hef'
            else
                echo '❌ No best.hef to copy'
                exit 1
            fi
        "
    
    if [[ -f "$HEF_OUT" ]]; then
        size_mb=$(du -h "$HEF_OUT" | cut -f1)
        echo ""
        echo "╔════════════════════════════════════════════════════════════════╗"
        echo "║ ✅ $MODEL_NAME complete: ${MODEL_NAME}_sprint13_vasco.hef ($size_mb)"
        echo "╚════════════════════════════════════════════════════════════════╝"
    else
        echo "❌ Failed: $HEF_OUT not found"
        exit 1
    fi
done

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║ ✅ ALL MODELS COMPLETE (Vasco's pipeline)                     ║"
echo "║ Generated HEFs:                                                ║"
ls -lh "$PHASE_E"/*_sprint13_vasco.hef 2>/dev/null | awk '{print "║   " $9 " (" $5 ")"}'
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
echo "Next: Compare with ONNX using:"
echo "  HEF_VARIANT_SUFFIX=_vasco bash run_compare_all4_onnx_vs_hef.sh"
echo ""
