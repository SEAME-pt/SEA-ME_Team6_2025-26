#!/usr/bin/env bash
set -euo pipefail

MODE="${1:-baseline}"          # baseline | custom
HW_ARCH="${HW_ARCH:-hailo8}"
SHARED="${SHARED:-/local/workspace/shared}"
CALIB_DIR="${CALIB_DIR:-$SHARED/calibration_images}"
COCO_DIR="${COCO_DIR:-$SHARED/COCO/val2017}"
MODEL_NAME="${MODEL_NAME:-yolov8n}"
ONNX_PATH="${ONNX_PATH:-$SHARED/models/yolov8n.onnx}"
PI_TARGET="${PI_TARGET:-}"     # ex: root@10.21.220.191:/root/models/

if ! command -v hailomz >/dev/null 2>&1; then
  echo "[ERROR] hailomz not found in PATH. Run this script inside the Hailo container."
  exit 1
fi

mkdir -p "$SHARED" "$SHARED/logs" "$SHARED/models" "$CALIB_DIR"

if [ "$(find "$CALIB_DIR" -type f | wc -l | tr -d ' ')" -eq 0 ]; then
  echo "[INFO] calibration_images is empty, copying up to 256 images from COCO..."
  if [ ! -d "$COCO_DIR" ]; then
    echo "[ERROR] COCO directory not found: $COCO_DIR"
    exit 1
  fi

  find "$COCO_DIR" -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" \) \
    | head -n 256 \
    | xargs -I{} cp "{}" "$CALIB_DIR"/
fi

echo "[INFO] Calibration images: $(find "$CALIB_DIR" -type f | wc -l | tr -d ' ')"

if [ "$MODE" = "baseline" ]; then
  hailomz compile "$MODEL_NAME" \
    --hw-arch "$HW_ARCH" \
    --calib-path "$CALIB_DIR" \
    2>&1 | tee "$SHARED/logs/compile_baseline.log"
elif [ "$MODE" = "custom" ]; then
  if [ ! -f "$ONNX_PATH" ]; then
    echo "[ERROR] ONNX file not found: $ONNX_PATH"
    exit 1
  fi

  hailomz compile "$MODEL_NAME" \
    --ckpt "$ONNX_PATH" \
    --hw-arch "$HW_ARCH" \
    --calib-path "$CALIB_DIR" \
    2>&1 | tee "$SHARED/logs/compile_custom.log"
else
  echo "[ERROR] Invalid mode: $MODE (use baseline|custom)"
  exit 1
fi

HEF_SOURCE="/local/workspace/${MODEL_NAME}.hef"
if [ ! -f "$HEF_SOURCE" ]; then
  echo "[ERROR] Expected HEF not found at $HEF_SOURCE"
  exit 1
fi

cp "$HEF_SOURCE" "$SHARED/models/"
cp "$HEF_SOURCE" "$SHARED/"
ls -lh "$SHARED/models/${MODEL_NAME}.hef"
ls -lh "$SHARED/${MODEL_NAME}.hef"

if [ -n "$PI_TARGET" ]; then
  echo "[INFO] Sending HEF to $PI_TARGET"
  scp "$SHARED/${MODEL_NAME}.hef" "$PI_TARGET"
fi

echo "[OK] Completed: mode=$MODE, model=$MODEL_NAME"
